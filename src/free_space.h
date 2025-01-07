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
    uint* free_indices; // Array of deleted node indices
    uint index_count;   // Number of free indices
} FreeSpace;

extern FreeSpace* free_space;

// Add status codes
#define FREE_SPACE_SUCCESS 0
#define FREE_SPACE_NEW 1
#define FREE_SPACE_ERROR -1

// Function declarations
int init_free_space(void);
void save_free_space(void);
FreeBlock* find_free_block(uint size);
void add_free_block(uint size, long offset);
int get_free_index(void);
void add_free_index(uint index);
void release_node_space(int node_index);
uchar* resize_node_space(uchar* node, ushort required_size, int node_index, uint* new_size);

#endif 