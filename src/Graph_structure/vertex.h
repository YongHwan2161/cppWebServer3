#ifndef VERTEX_H
#define VERTEX_H

#include "../../CGDB.h"
#include <stdbool.h>
Vertices get_connected_vertices(Vertex vertex, ushort axis_number);
Vertices get_parent_vertices(Vertex vertex);
Vertices get_child_vertices(Vertex vertex);
Vertex get_next_vertex(unsigned int node_index, unsigned short channel, unsigned short axis_number, ushort link_index);
Vertex get_next_vertex_check(unsigned int node_index, unsigned short channel, unsigned short axis_number, ushort link_index);
int migrate_parent_vertices(Vertex source_vertex, Vertex target_vertex);
int migrate_child_vertices(Vertex source_vertex, Vertex target_vertex);
bool is_start_string_vertex(Vertex vertex);
bool is_root_vertex(Vertex vertex);
int move_current_vertex(Vertex vertex);
int handle_move_current_vertex(char* args);
int load_current_vertex();
int load_root_vertex();
int update_current_vertex();
int update_root_vertex();
int update_current_vertex_to_root();
int handle_get_current_vertex();
int handle_get_root_vertex();
#endif
