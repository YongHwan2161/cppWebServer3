#include "map.h"
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
    
    // Write number of nodes
    uint num_nodes = 256;
    fwrite(&num_nodes, sizeof(uint), 1, map_file);
    
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
    
    // Read and verify number of nodes
    uint num_nodes;
    if (fread(&num_nodes, sizeof(uint), 1, map_file) != 1 || num_nodes != 256) {
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