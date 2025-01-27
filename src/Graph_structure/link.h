#ifndef LINK_H
#define LINK_H

#include "../../CGDB.h"
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
int create_link(uint source_node, ushort source_ch, uint dest_node, ushort dest_ch, ushort axis_number);
int create_loop(uint source_node, ushort source_ch, ushort axis_number);
int create_bidirectional_link(Vertex source_vertex, Vertex dest_vertex);
int create_forward_link(Vertex source_vertex, Vertex dest_vertex);
int create_property(uint node_index, ushort channel_index, uint property_node);
int delete_link(uint source_node, ushort source_ch, uint dest_node, ushort dest_ch, ushort axis_number);
int delete_first_link(uint source_node, ushort source_ch, ushort axis_number);
int get_link(uint source_node, ushort source_ch, ushort axis_number, ushort link_index, uint* dest_node, ushort* dest_ch);
int get_link_count(uint source_node, ushort source_ch, ushort axis_number);
int get_property(uint node_index, ushort channel_index);

#endif // LINK_H 