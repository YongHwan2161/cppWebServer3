#include "map.h"
#include "memory.h"
#include "../CGDB.h"
#include <stdio.h>

int save_map(uint node_index) {
    FILE* map_file = fopen(MAP_FILE, "r+b");
    if (!map_file) {
        printf("Error: Could not open map file for writing\n");
        return MAP_ERROR;
    }
    
    // Calculate exact position for this node's file_offset
    long position = sizeof(uint) + (node_index * sizeof(long));
    
    // Seek to exact position and write only the file_offset
    if (fseek(map_file, position, SEEK_SET) != 0) {
        printf("Error: Failed to seek in map file\n");
        fclose(map_file);
        return MAP_ERROR;
    }
    
    // Write only the file_offset value
    size_t written = fwrite(&CoreMap[node_index].file_offset, sizeof(long), 1, map_file);
    fclose(map_file);
    
    if (written != 1) {
        printf("Error: Failed to write to map file\n");
        return MAP_ERROR;
    }
    
    return MAP_SUCCESS;
}

int save_map_all(void) {
    FILE* map_file = fopen(MAP_FILE, "r+b");
    if (!map_file) {
        printf("Error: Could not open map file for writing\n");
        return MAP_ERROR;
    }
    
    // Write number of vertices
    uint num_vertices = 256;
    fwrite(&num_vertices, sizeof(uint), 1, map_file);
    
    // Write all offsets
    for (int i = 0; i < 256; i++) {
        fwrite(&CoreMap[i].file_offset, sizeof(long), 1, map_file);
    }
    
    fclose(map_file);
    return MAP_SUCCESS;
}

// int load_map(void) {
//     FILE* map_file = fopen(MAP_FILE, "rb");
//     if (!map_file) {
//         printf("Error: Could not open map file for reading\n");
//         return MAP_ERROR;
//     }
    
//     // Read and verify number of vertices
//     uint num_vertices;
//     if (fread(&num_vertices, sizeof(uint), 1, map_file) != 1 || num_vertices != 256) {
//         fclose(map_file);
//         return MAP_ERROR;
//     }
    
//     // Read all offsets
//     for (int i = 0; i < 256; i++) {
//         if (fread(&CoreMap[i].file_offset, sizeof(long), 1, map_file) != 1) {
//             fclose(map_file);
//             return MAP_ERROR;
//         }
//     }
    
//     fclose(map_file);
//     return MAP_SUCCESS;
// }

void init_map(void) {
    // Initialize CoreMap with default values
    for (int i = 0; i < 256; i++) {
        CoreMap[i].core_position = -1;
        CoreMap[i].is_loaded = 0;
        CoreMap[i].file_offset = 0;
    }
}

long get_node_position(unsigned int node_index) {
    // printf("calling get_node_position: %d\n", node_index);
    if (node_index >= MaxCoreSize) {
        printf("Error: node index %d is out of bounds\n", node_index);
        return -1;
    }
    // Check if node is loaded
    if (!CoreMap[node_index].is_loaded) {
        printf("node is not loaded\n");
        int position = load_node_to_core(node_index);
        if (position == -1) {
            printf("Error: node %d is not loaded in memory\n", node_index);
            return -1;
        }
    }
    
    return CoreMap[node_index].core_position;
} 
long get_last_offset() {
    // long last_offset = 0;

    // size_t node_size = (size_t)1 << *(ushort*)Core[max_offset_node];
    // last_offset += (long)node_size;
    return max_offset;
}
long get_last_offset_by_induction(void) {
    long last_offset = 0;
    unsigned int max_node_index = 0;

    for (unsigned int i = 0; i < CurrentnodeCount; i++) {
        if (CoreMap[i].file_offset > last_offset) {
            last_offset = CoreMap[i].file_offset;
            max_node_index = i;
        }
    }
    size_t node_size = (size_t)1 << *(ushort*)Core[max_node_index];
    last_offset += (long)node_size;
    return last_offset;
}