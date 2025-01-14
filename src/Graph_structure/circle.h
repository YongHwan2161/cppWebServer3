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
bool has_circle(int vertex_index, int channel_index, int axis_number);

// Get detailed information about the circle
CircleInfo* get_circle_info(int vertex_index, int channel_index, int axis_number);

// Free circle info structure
void free_circle_info(CircleInfo* info);

#endif // CIRCLE_H 