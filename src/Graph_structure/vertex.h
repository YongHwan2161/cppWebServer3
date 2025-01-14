#ifndef vertex_H
#define vertex_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
// Function declarations
bool validate_vertex(unsigned int vertex_index);
bool save_vertex_to_file(unsigned int vertex_index);
void create_new_vertex();
void delete_vertex(unsigned int vertex_index);

#endif // vertex_H 