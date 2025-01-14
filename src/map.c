#include "map.h"
#include "memory.h"
#include <stdio.h>

int save_map(uint vertex_index) {
    FILE* map_file = fopen(MAP_FILE, "r+b");
    if (!map_file) {
        printf("Error: Could not open map file for writing\n");
        return MAP_ERROR;
    }
    
    // Calculate exact position for this vertex's file_offset
    long position = sizeof(uint) + (vertex_index * sizeof(long));
    
    // Seek to exact position and write only the file_offset
    if (fseek(map_file, position, SEEK_SET) != 0) {
        printf("Error: Failed to seek in map file\n");
        fclose(map_file);
        return MAP_ERROR;
    }
    
    // Write only the file_offset value
    size_t written = fwrite(&CoreMap[vertex_index].file_offset, sizeof(long), 1, map_file);
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

int load_map(void) {
    FILE* map_file = fopen(MAP_FILE, "rb");
    if (!map_file) {
        printf("Error: Could not open map file for reading\n");
        return MAP_ERROR;
    }
    
    // Read and verify number of vertices
    uint num_vertices;
    if (fread(&num_vertices, sizeof(uint), 1, map_file) != 1 || num_vertices != 256) {
        fclose(map_file);
        return MAP_ERROR;
    }
    
    // Read all offsets
    for (int i = 0; i < 256; i++) {
        if (fread(&CoreMap[i].file_offset, sizeof(long), 1, map_file) != 1) {
            fclose(map_file);
            return MAP_ERROR;
        }
    }
    
    fclose(map_file);
    return MAP_SUCCESS;
}

void init_map(void) {
    // Initialize CoreMap with default values
    for (int i = 0; i < 256; i++) {
        CoreMap[i].core_position = -1;
        CoreMap[i].is_loaded = 0;
        CoreMap[i].file_offset = 0;
    }
}

int get_vertex_position(unsigned int vertex_index) {
    // printf("calling get_vertex_position: %d\n", vertex_index);
    // Check if vertex is loaded
    if (!CoreMap[vertex_index].is_loaded) {
        printf("vertex is not loaded\n");
        int position = load_vertex_to_core(vertex_index);
        if (position == -1) {
            printf("Error: vertex %d is not loaded in memory\n", vertex_index);
            return -1;
        }
    }
    
    return CoreMap[vertex_index].core_position;
} 