#include "memory.h"

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