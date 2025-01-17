#include "vertex.h"
#include "../../CGDB.h"
#include "channel.h"
#include "axis.h"
#include <stdio.h>
#include "link.h"  // For Core array access
#include "cycle.h"
#include "../map.h"
// Initial vertex values
static uchar initValues[16] = {
    4,  0,     // allocated size power (2^4 = 16 bytes)
    14, 0, 0, 0,  // actual used size (14 bytes initially)
    1,  0,     // number of channels (1)
    12, 0, 0, 0,   // offset for channel 0 (starts at byte 12)
    0,  0,     // number of axes (0)
    0,  0      // remaining bytes initialized to 0
};

/**
 * Validates if a vertex index is valid and the vertex exists in Core
 * 
 * @param vertex_index Index of the vertex to validate
 * @return true if valid, false if invalid
 */
bool validate_vertex(unsigned int vertex_index) {
    // Check if vertex exists in Core
    uint vertex_position = get_vertex_position(vertex_index);
    if (!Core[vertex_position]) {
        printf("Error: Invalid vertex index %u\n", vertex_index);
        return false;
    }

    return true;
}

/**
 * Saves vertex data to data.bin file and updates map.bin
 * 
 * @param vertex_index Index of the vertex to save
 * @return true if successful, false if failed
 */
bool save_vertex_to_file(unsigned int vertex_index) {
    if (!validate_vertex(vertex_index)) {
        return false;
    }
    // printf("start save_vertex_to_file\n");
    uint vertex_position = CoreMap[vertex_index].core_position;
    uchar* vertex = Core[vertex_position];
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
    printf("file offset: 0x%lx\n", CoreMap[vertex_index].file_offset);
    // Write vertex data
    if (fseek(data_file, CoreMap[vertex_index].file_offset, SEEK_SET) != 0) {
        printf("Error: Failed to seek in data.bin\n");
        fclose(data_file);
        return false;
    }

    size_t vertex_size = 1 << (*(ushort*)vertex);
    if (fwrite(vertex, 1, vertex_size, data_file) != vertex_size) {
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
        // Write initial vertex count
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
    if (fseek(map_file, sizeof(uint) + (vertex_index * sizeof(long)), SEEK_SET) != 0) {
        printf("Error: Failed to seek in map.bin\n");
        fclose(map_file);
        return false;
    }

    if (fwrite(&CoreMap[vertex_index].file_offset, sizeof(long), 1, map_file) != 1) {
        printf("Error: Failed to write to map.bin\n");
        fclose(map_file);
        return false;
    }
    
    fclose(map_file);
    return true;
}

bool save_current_vertex_count() {
    FILE* map_file = fopen(MAP_FILE, "r+b");
    if (!map_file) {
        printf("Error: Failed to open map.bin in save_current_vertex_count\n");
        map_file = fopen(MAP_FILE, "wb");
        if (!map_file) {
            printf("Error: Failed to create map.bin in save_current_vertex_count\n");
            return false;
        }
    }
    fwrite(&CurrentvertexCount, sizeof(uint), 1, map_file);
    fclose(map_file);
    return true;
}
void initialize_vertex(uchar** vertex) {
    for (int i = 0; i < 16; ++i) {
        (*vertex)[i] = initValues[i];
    }
}
void create_new_vertex() {
    uchar* newvertex = (uchar*)malloc(16 * sizeof(uchar));  // Always allocate 16 bytes initially
    // printf("Creating new vertex at index %d\n", CurrentvertexCount);
    initialize_vertex(&newvertex);
    CurrentvertexCount++;
    save_current_vertex_count();
    Core[CurrentvertexCount - 1] = newvertex;
    CoreSize++;
    CoreMap[CurrentvertexCount - 1].core_position = CurrentvertexCount - 1;
    CoreMap[CurrentvertexCount - 1].is_loaded = 1;
    if (CurrentvertexCount == 1) {
        CoreMap[CurrentvertexCount - 1].file_offset = 0;
    } else {
        uint last_vertex_size = 1 << (*(ushort*)Core[CurrentvertexCount - 2]);
        // printf("Last vertex size: %d\n", last_vertex_size);
        uint file_offset = CoreMap[CurrentvertexCount - 2].file_offset + last_vertex_size;
        CoreMap[CurrentvertexCount - 1].file_offset = file_offset;
    }
    save_vertex_to_file(CurrentvertexCount - 1);
    printf("vertex created at index %d\n", CurrentvertexCount - 1);
}
int delete_vertex(unsigned int vertex_index) {
    printf("calling delete_vertex: %d\n", vertex_index);
    // Check if trying to delete garbage vertex
    if (vertex_index == GarbagevertexIndex) {
        return VERTEX_ERROR_GARBAGE;
    }
    
    // Check if vertex is in garbage cycle
    if (is_in_garbage_cycle(vertex_index)) {
        return VERTEX_ERROR_IN_GARBAGE_cycle;
    }
    
    uint vertex_position = CoreMap[vertex_index].core_position;
    printf("vertex_position: %d\n", vertex_position);
    initialize_vertex(&Core[vertex_position]);
    printf("vertex initialized\n");

    uint channel_offset = get_channel_offset(Core[GarbagevertexIndex], 0);
    uint axis_offset = get_axis_offset(Core[GarbagevertexIndex], 0, 0);
    uint first_garbage_vertex = *(uint*)(Core[GarbagevertexIndex] + channel_offset + axis_offset + 2);
    printf("First garbage vertex: %d\n", first_garbage_vertex);
    delete_link(GarbagevertexIndex, 0, first_garbage_vertex, 0, 0);
    create_link(GarbagevertexIndex, 0, vertex_index, 0, 0);
    create_link(vertex_index, 0, first_garbage_vertex, 0, 0);

    Core[vertex_position] = NULL;
    CoreMap[vertex_index].core_position = -1;
    CoreMap[vertex_index].is_loaded = 0;

    CoreSize--;
    return VERTEX_SUCCESS;
}