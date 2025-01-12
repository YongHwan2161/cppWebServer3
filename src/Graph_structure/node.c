#include "node.h"
#include "../../CGDB.h"
#include <stdio.h>
#include "link.h"  // For Core array access

// Initial node values
static uchar initValues[16] = {
    4,  0,     // allocated size power (2^4 = 16 bytes)
    14, 0, 0, 0,  // actual used size (14 bytes initially)
    1,  0,     // number of channels (1)
    12, 0, 0, 0,   // offset for channel 0 (starts at byte 12)
    0,  0,     // number of axes (0)
    0,  0      // remaining bytes initialized to 0
};

/**
 * Validates if a node index is valid and the node exists in Core
 * 
 * @param node_index Index of the node to validate
 * @return true if valid, false if invalid
 */
bool validate_node(unsigned int node_index) {
    // Check if node exists in Core
    if (!Core[node_index]) {
        printf("Error: Invalid node index %u\n", node_index);
        return false;
    }

    return true;
}

/**
 * Saves node data to data.bin file and updates map.bin
 * 
 * @param node_index Index of the node to save
 * @return true if successful, false if failed
 */
bool save_node_to_file(unsigned int node_index) {
    if (!validate_node(node_index)) {
        return false;
    }
    uchar* node = Core[node_index];
    // Try to open data file, create if doesn't exist
    FILE* data_file = fopen(DATA_FILE, "r+b");
    if (!data_file) {
        data_file = fopen(DATA_FILE, "wb");
        if (!data_file) {
            printf("Error: Failed to create data.bin\n");
            return false;
        }
        fclose(data_file);
        data_file = fopen(DATA_FILE, "r+b");
    }

    // Write node data
    if (fseek(data_file, CoreMap[node_index].file_offset, SEEK_SET) != 0) {
        printf("Error: Failed to seek in data.bin\n");
        fclose(data_file);
        return false;
    }

    size_t node_size = 1 << (*(ushort*)node);
    if (fwrite(node, 1, node_size, data_file) != node_size) {
        printf("Error: Failed to write to data.bin\n");
        fclose(data_file);
        return false;
    }
    fclose(data_file);

    // Try to open map file, create if doesn't exist
    FILE* map_file = fopen(MAP_FILE, "r+b");
    if (!map_file) {
        map_file = fopen(MAP_FILE, "wb");
        if (!map_file) {
            printf("Error: Failed to create map.bin\n");
            return false;
        }
        // // Write initial node count
        // uint initial_count = 256;
        // fwrite(&initial_count, sizeof(uint), 1, map_file);
        // // Initialize all offsets to 0
        // long zero_offset = 0;
        // for (int i = 0; i < 256; i++) {
        //     fwrite(&zero_offset, sizeof(long), 1, map_file);
        // }
        fclose(map_file);
        map_file = fopen(MAP_FILE, "r+b");
    }

    // Update map entry
    if (fseek(map_file, sizeof(uint) + (node_index * sizeof(long)), SEEK_SET) != 0) {
        printf("Error: Failed to seek in map.bin\n");
        fclose(map_file);
        return false;
    }

    if (fwrite(&CoreMap[node_index].file_offset, sizeof(long), 1, map_file) != 1) {
        printf("Error: Failed to write to map.bin\n");
        fclose(map_file);
        return false;
    }
    
    fclose(map_file);
    return true;
}

bool save_node_to_file2(FILE* data_file, FILE* map_file, unsigned int index) {
    uchar* node = Core[index];
    long offset = ftell(data_file);
    
    fwrite(&offset, sizeof(long), 1, map_file);
    
    // Read size as power of 2 (first 2 bytes)
    ushort size_power = *(ushort*)node;
    uint actual_size = 1 << size_power;  // 2^size_power
    
    // Write the entire node data
    fwrite(node, sizeof(uchar), actual_size, data_file);
    return true;
}
bool save_current_node_count() {
    FILE* map_file = fopen(MAP_FILE, "r+b");
    if (!map_file) {
        printf("Error: Failed to open map.bin\n");
        return false;
    }
    fwrite(&CurrentNodeCount, sizeof(uint), 1, map_file);
    fclose(map_file);
    return true;
}
void create_new_node() {
    uchar* newNode = (uchar*)malloc(16 * sizeof(uchar));  // Always allocate 16 bytes initially
    // printf("Creating new node at index %d\n", CurrentNodeCount);
    for (int i = 0; i < 16; ++i) {
        newNode[i] = initValues[i];
    }
    CurrentNodeCount++;
    save_current_node_count();
    Core[CurrentNodeCount - 1] = newNode;
    CoreSize++;
    CoreMap[CurrentNodeCount - 1].core_position = CurrentNodeCount - 1;
    CoreMap[CurrentNodeCount - 1].is_loaded = 1;
    uint last_node_size = 1 << (*(ushort*)Core[CurrentNodeCount - 1]);
    // printf("Last node size: %d\n", last_node_size);
    uint file_offset = CoreMap[CurrentNodeCount - 1].file_offset + last_node_size;
    CoreMap[CurrentNodeCount - 1].file_offset = file_offset;
    save_node_to_file(CurrentNodeCount - 1);
    printf("Node created at index %d\n", CurrentNodeCount - 1);
}