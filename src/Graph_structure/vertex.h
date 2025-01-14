#ifndef vertex_H
#define vertex_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
// Function declarations
bool validate_vertex(unsigned int vertex_index);
bool save_vertex_to_file(unsigned int vertex_index);
void create_new_vertex();
int delete_vertex(unsigned int vertex_index);

// Add these error code definitions
#define VERTEX_SUCCESS 0
#define VERTEX_ERROR_GARBAGE 1
#define VERTEX_ERROR_IN_GARBAGE_CIRCLE 2

#endif // vertex_H 