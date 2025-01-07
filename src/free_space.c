#include "free_space.h"
#include <string.h>

FreeSpace* free_space = NULL;

int init_free_space() {
    free_space = (FreeSpace*)malloc(sizeof(FreeSpace));
    if (!free_space) return FREE_SPACE_ERROR;
    
    free_space->count = 0;
    free_space->blocks = NULL;
    free_space->free_indices = NULL;
    free_space->index_count = 0;
    
    // Try to load existing free space data
    FILE* fs_file = fopen(FREE_SPACE_FILE, "rb");
    if (fs_file) {
        // Read block count
        if (fread(&free_space->count, sizeof(uint), 1, fs_file) != 1) {
            fclose(fs_file);
            return FREE_SPACE_ERROR;
        }
        
        if (free_space->count > 0) {
            free_space->blocks = (FreeBlock*)malloc(free_space->count * sizeof(FreeBlock));
            if (!free_space->blocks) {
                fclose(fs_file);
                return FREE_SPACE_ERROR;
            }
            if (fread(free_space->blocks, sizeof(FreeBlock), free_space->count, fs_file) 
                != free_space->count) {
                fclose(fs_file);
                return FREE_SPACE_ERROR;
            }
        }
        
        // Read free indices
        if (fread(&free_space->index_count, sizeof(uint), 1, fs_file) != 1) {
            fclose(fs_file);
            return FREE_SPACE_ERROR;
        }
        
        if (free_space->index_count > 0) {
            free_space->free_indices = (uint*)malloc(free_space->index_count * sizeof(uint));
            if (!free_space->free_indices) {
                fclose(fs_file);
                return FREE_SPACE_ERROR;
            }
            if (fread(free_space->free_indices, sizeof(uint), free_space->index_count, fs_file) 
                != free_space->index_count) {
                fclose(fs_file);
                return FREE_SPACE_ERROR;
            }
        }
        
        fclose(fs_file);
        return FREE_SPACE_SUCCESS;
    }
    
    // No existing file - create new
    save_free_space();
    return FREE_SPACE_NEW;
}

void save_free_space() {
    FILE* fs_file = fopen(FREE_SPACE_FILE, "wb");
    if (!fs_file) return;
    
    fwrite(&free_space->count, sizeof(uint), 1, fs_file);
    if (free_space->count > 0) {
        fwrite(free_space->blocks, sizeof(FreeBlock), free_space->count, fs_file);
    }
    
    fwrite(&free_space->index_count, sizeof(uint), 1, fs_file);
    if (free_space->index_count > 0) {
        fwrite(free_space->free_indices, sizeof(uint), free_space->index_count, fs_file);
    }
    
    fclose(fs_file);
}

FreeBlock* find_free_block(uint size) {
    for (uint i = 0; i < free_space->count; i++) {
        if (free_space->blocks[i].size == size) {
            return &free_space->blocks[i];
        }
    }
    return NULL;
}

void add_free_block(uint size, long offset) {
    free_space->count++;
    free_space->blocks = (FreeBlock*)realloc(free_space->blocks, 
                                            free_space->count * sizeof(FreeBlock));
    
    free_space->blocks[free_space->count - 1].size = size;
    free_space->blocks[free_space->count - 1].offset = offset;
}

int get_free_index() {
    if (free_space->index_count == 0) return -1;
    
    int index = free_space->free_indices[free_space->index_count - 1];
    free_space->index_count--;
    
    if (free_space->index_count == 0) {
        free(free_space->free_indices);
        free_space->free_indices = NULL;
    } else {
        free_space->free_indices = (uint*)realloc(free_space->free_indices,
                                                 free_space->index_count * sizeof(uint));
    }
    
    return index;
}

void add_free_index(uint index) {
    free_space->index_count++;
    free_space->free_indices = (uint*)realloc(free_space->free_indices,
                                             free_space->index_count * sizeof(uint));
    free_space->free_indices[free_space->index_count - 1] = index;
}

void release_node_space(int node_index) {
    if (!CoreMap[node_index].is_loaded) return;
    
    // Get node size
    uchar* node = Core[CoreMap[node_index].core_position];
    ushort size_power = *(ushort*)node;
    uint actual_size = 1 << size_power;
    
    // Add to free space
    add_free_block(actual_size, CoreMap[node_index].file_offset);
    
    // Clear data in data.bin
    FILE* data_file = fopen(DATA_FILE, "r+b");
    if (data_file) {
        fseek(data_file, CoreMap[node_index].file_offset, SEEK_SET);
        uchar* zeros = (uchar*)calloc(actual_size, 1);
        fwrite(zeros, 1, actual_size, data_file);
        free(zeros);
        fclose(data_file);
    }
    
    // Add index to free indices
    add_free_index(node_index);
    
    // Update free space file
    save_free_space();
}

uchar* resize_node_space(uchar* node, ushort required_size, int node_index, uint* new_size) {
    // Calculate new size (next power of 2)
    ushort node_size_power = *(ushort*)node;
    uint current_size = 1 << node_size_power;
    
    while ((1 << node_size_power) < required_size) {
        node_size_power++;
    }
    
    // Set new size for caller
    *new_size = 1 << node_size_power;
    
    // Allocate new space
    FreeBlock* free_block = find_free_block(*new_size);
    uchar* new_node = NULL;
    
    if (free_block) {
        // Use found free block
        new_node = (uchar*)malloc(*new_size);
        memcpy(new_node, node, current_size);
        
        // Update CoreMap with new location
        CoreMap[node_index].file_offset = free_block->offset;
        
        // Add old space to free space
        add_free_block(current_size, CoreMap[node_index].file_offset);
    } else {
        // No suitable free block found, allocate at end of file
        new_node = (uchar*)malloc(*new_size);
        memcpy(new_node, node, current_size);
        
        // Add old space to free space
        add_free_block(current_size, CoreMap[node_index].file_offset);
        
        // Update CoreMap with new location
        FILE* data_file = fopen(DATA_FILE, "ab");
        if (data_file) {
            CoreMap[node_index].file_offset = ftell(data_file);
            fclose(data_file);
        }
    }
    
    // Update node size
    *(ushort*)new_node = node_size_power;
    
    // Free old node
    free(node);
    
    return new_node;
} 