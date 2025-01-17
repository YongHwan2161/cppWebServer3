#ifndef cycle_H
#define cycle_H

#include <stdbool.h>
#include "../../CGDB.h"
// Structure to store cycle information
typedef struct {
    uint* vertices;         // Array of vertices in cycle
    ushort* channels;    // Array of channels in cycle
    int count;          // Number of vertices in cycle
} cycleInfo;

// Check if there is a cycle starting from given vertex/channel/axis
bool has_cycle(unsigned int vertex_index, ushort channel_index, ushort axis_number);

// Get detailed information about the cycle
cycleInfo* get_cycle_info(unsigned int vertex_index, ushort channel_index, ushort axis_number);

// Free cycle info structure
void free_cycle_info(cycleInfo* info);

// Add this function declaration
bool is_in_garbage_cycle(unsigned int vertex_index);

// Add this function declaration
int create_cycle(uint* vertices, ushort* channels, int count, ushort axis_number);

#endif // cycle_H 