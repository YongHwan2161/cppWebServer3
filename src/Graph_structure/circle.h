#ifndef CIRCLE_H
#define CIRCLE_H

#include <stdbool.h>
#include "../../CGDB.h"
// Structure to store circle information
typedef struct {
    uint* vertices;         // Array of vertices in circle
    ushort* channels;    // Array of channels in circle
    int count;          // Number of vertices in circle
} CircleInfo;

// Check if there is a circle starting from given vertex/channel/axis
bool has_circle(unsigned int vertex_index, ushort channel_index, ushort axis_number);

// Get detailed information about the circle
CircleInfo* get_circle_info(unsigned int vertex_index, ushort channel_index, ushort axis_number);

// Free circle info structure
void free_circle_info(CircleInfo* info);

// Add this function declaration
bool is_in_garbage_circle(unsigned int vertex_index);

#endif // CIRCLE_H 