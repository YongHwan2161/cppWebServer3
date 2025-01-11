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
#define FREE_SPACE_RESIZED 2
// Function declarations
int init_free_space(void);
void save_free_space(void);
FreeBlock* find_free_block(uint size);
FreeBlock* find_and_get_free_block(uint size);
void add_free_block(uint size, long offset);
int get_free_index(void);
void add_free_index(uint index);
void release_node_space(int node_index);
uchar* resize_node_space(uchar* node, ushort required_size, uint node_index, uint* new_size);

/**
 * Checks if node needs resizing and resizes if necessary
 * 
 * @param node_ptr Pointer to node pointer (may be updated if resized)
 * @param required_size Required size for the node
 * @param node_index Index of the node
 * @return FREE_SPACE_SUCCESS if no resize needed, 
 *         FREE_SPACE_RESIZED if resized successfully,
 *         FREE_SPACE_ERROR if resize failed
 */
int check_and_resize_node(uchar* node, uint required_size, uint node_index);

#endif 