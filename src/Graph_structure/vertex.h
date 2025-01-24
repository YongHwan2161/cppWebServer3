#ifndef VERTEX_H
#define VERTEX_H

#include "../../CGDB.h"
#include <stdbool.h>
Vertices get_connected_vertices(Vertex vertex, ushort axis_number);
Vertices get_parent_vertices(Vertex vertex);
Vertices get_child_vertices(Vertex vertex);
int migrate_parent_vertices(Vertex source_vertex, Vertex target_vertex);
int migrate_child_vertices(Vertex source_vertex, Vertex target_vertex);
bool is_start_string_vertex(Vertex vertex);

#endif