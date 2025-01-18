#ifndef LINK_H
#define LINK_H

#include "../../CGDB.h"
#include "axis.h"

// Link operation results
#define LINK_SUCCESS 0
#define LINK_ERROR -1

// Link structure (6 bytes)
typedef struct {
    uint vertex;     // 4 bytes for vertex index
    ushort channel; // 2 bytes for channel index
} Link;

// Function declarations
int create_link(uint source_vertex, ushort source_ch, uint dest_vertex, ushort dest_ch, ushort axis_number);
int create_loop(uint source_vertex, ushort source_ch, ushort axis_number);
int create_bidirectional_link(uint source_vertex, ushort source_ch, uint dest_vertex, ushort dest_ch);
int create_forward_link(uint source_vertex, ushort source_ch, uint dest_vertex, ushort dest_ch);
int create_backward_link(uint source_vertex, ushort source_ch, uint dest_vertex, ushort dest_ch);
int delete_link(uint source_vertex, ushort source_ch, uint dest_vertex, ushort dest_ch, ushort axis_number);

#endif // LINK_H 