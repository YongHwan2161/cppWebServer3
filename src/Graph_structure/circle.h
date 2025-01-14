#ifndef CIRCLE_H
#define CIRCLE_H

#include <stdbool.h>
#include "../../CGDB.h"
// Structure to store circle information
typedef struct {
    uint* nodes;         // Array of nodes in circle
    ushort* channels;    // Array of channels in circle
    int count;          // Number of nodes in circle
} CircleInfo;

// Check if there is a circle starting from given node/channel/axis
bool has_circle(int node_index, int channel_index, int axis_number);

// Get detailed information about the circle
CircleInfo* get_circle_info(int node_index, int channel_index, int axis_number);

// Free circle info structure
void free_circle_info(CircleInfo* info);

#endif // CIRCLE_H 