#ifndef FREE_SPACE_H
#define FREE_SPACE_H

#include "../CGDB.h"

#define FREE_SPACE_FILE "binary-data/free_space.bin"

typedef struct {
    uint size;          // Size of free block (power of 2)
    long offset;        // Offset in data.bin
} FreeBlock;

typedef struct {
    uint count;         // Number of free blocks
    FreeBlock* blocks;  // Array of free blocks
} FreeSpace;

extern FreeSpace* free_space;

// Add status codes
#define FREE_SPACE_SUCCESS 0
#define FREE_SPACE_NEW 1
#define FREE_SPACE_ERROR -1
#define FREE_SPACE_RESIZED 2

// Function declarations
int init_free_space(void);
void save_free_space(void);
FreeBlock* find_free_block(uint size);
FreeBlock* find_and_get_free_block(uint size);
void add_free_block(uint size, long offset);
void release_vertex_space(int vertex_index);
uchar* resize_vertex_space(uchar* vertex, ushort required_size, uint vertex_index, uint* new_size);
int check_and_resize_vertex(uchar* vertex, uint required_size, uint vertex_index);

#endif 