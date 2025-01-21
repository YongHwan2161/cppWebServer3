#include "memory.h"
#include "Graph_structure/node.h"
#include "../CGDB.h"
#include "map.h"

static inline void move_data_forward(unsigned char* dest, unsigned int pos, 
                                   unsigned int size, unsigned int move_size) {
    if (move_size > 0) {
        memmove(dest + pos + size,    // New position
                dest + pos,           // Current position
                move_size);           // Amount to move
    }
}
static inline void move_data_backward(unsigned char* dest, unsigned int pos, 
                                     unsigned int size, unsigned int move_size) {
    if (move_size > 0) {
        memmove(dest + pos - size,    // New position
                dest + pos,                 // Current position
                move_size);                 // Amount to move
    }
}
int insert_memory(unsigned char* dest, unsigned int insert_pos,
                 const unsigned char* src, unsigned int src_size, 
                 unsigned int total_size, unsigned int move_size) {
                     
    // Validate parameters
    if (!dest || !src) return 0;
    if (insert_pos + src_size + move_size > total_size) return 0;
    
    // Move and insert data
    move_data_forward(dest, insert_pos, src_size, move_size);
    memcpy(dest + insert_pos, src, src_size);
    
    return 1;
}

int insert_axis_entry(unsigned char* dest, unsigned int insert_pos,
                     unsigned short axis_number, unsigned int axis_offset,
                     unsigned int move_size) {
    
    // Validate parameters
    if (!dest) return 0;
    
    // Move and insert data
    move_data_forward(dest, insert_pos, 6, move_size);
    
    // Insert axis data in one operation using a struct
    struct {
        unsigned short number;
        unsigned int offset;
    } __attribute__((packed)) axis_data = {axis_number, axis_offset};
    
    memcpy(dest + insert_pos, &axis_data, sizeof(axis_data));
    
    return 1;
}

int insert_ushort(unsigned char* dest, unsigned int insert_pos,
                 unsigned short value, unsigned int move_size) {
    
    // Validate parameters
    if (!dest) return 0;
    
    // Move and insert data
    move_data_forward(dest, insert_pos, 2, move_size);
    memcpy(dest + insert_pos, &value, sizeof(value));
    
    return 1;
}

int insert_uint(unsigned char* dest, unsigned int insert_pos,
               unsigned int value, unsigned int move_size) {
    
    // Validate parameters
    if (!dest) return 0;
    
    // Move and insert data
    move_data_forward(dest, insert_pos, 4, move_size);
    memcpy(dest + insert_pos, &value, sizeof(value));
    
    return 1;
}

int insert_link(unsigned char* dest, unsigned int insert_pos,
               unsigned int node_index, unsigned short channel_index,
               unsigned int move_size) {
    
    // Validate parameters
    if (!dest) return 0;
    
    // Move and insert data
    if (move_size > 0) {
        move_data_forward(dest, insert_pos, 6, move_size);
    }
    
    // Insert link data in one operation using a struct
    struct {
        unsigned int node;
        unsigned short channel;
    } __attribute__((packed)) link_data = {node_index, channel_index};
    
    memcpy(dest + insert_pos, &link_data, sizeof(link_data));
    
    return 1;
}

int unload_node_data(unsigned int node_index) {
    // Validate node index
    if (node_index >= 256) {
        printf("Error: Invalid node index %d\n", node_index);
        return 0;
    }
    
    // Get node position
    int position = get_node_position(node_index);
    if (position < 0) {
        return 0;  // Error already printed by get_node_position
    }
    
    // Free the node memory
    free(Core[position]);
    Core[position] = NULL;
    
    // Update CoreMap
    CoreMap[node_index].is_loaded = 0;
    CoreMap[node_index].core_position = -1;
    
    // Decrement core size
    CoreSize--;
    
    return 1;
} 
void load_node_from_file(FILE* data_file, long offset, unsigned int index) {
    fseek(data_file, offset, SEEK_SET);
    
    // Read size power first (2 bytes)
    unsigned short size_power;
    fread(&size_power, sizeof(unsigned short), 1, data_file);
    
    // Calculate actual size
    unsigned int actual_size = 1 << size_power;
    
    // Allocate memory for the node
    uchar* newnode = (uchar*)malloc(actual_size * sizeof(uchar));
    
    // Move back 2 bytes instead of seeking from start
    fseek(data_file, -2, SEEK_CUR);
    fread(newnode, sizeof(uchar), actual_size, data_file);
    
    Core[index] = newnode;
}
int load_node_to_core(unsigned int node_index) {

    FILE* data_file = fopen(DATA_FILE, "rb");
    if (!data_file) return -1;
    
    // Use stored offset directly
    long offset = CoreMap[node_index].file_offset;
    
    // Load the node
    for (uint i = 0; i < MaxCoreSize; i++) {
        if (Core[i] == NULL) {
            CoreMap[node_index].core_position = i;
            CoreMap[node_index].is_loaded = 1;
            load_node_from_file(data_file, offset, i);
            break;
        }
    }
    CoreSize++;
    
    fclose(data_file);
    return CoreMap[node_index].core_position;
}