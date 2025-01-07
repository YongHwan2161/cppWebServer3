#ifndef LINK_H
#define LINK_H

#include "../CGDB.h"
#include "axis.h"

// Link operation results
#define LINK_SUCCESS 0
#define LINK_ERROR -1

// Link structure (6 bytes)
typedef struct {
    uint node;     // 4 bytes for node index
    ushort channel; // 2 bytes for channel index
} Link;

// Function declarations
int create_link(int source_node, int source_ch, 
               int dest_node, int dest_ch, 
               int axis_number);

#endif // LINK_H 