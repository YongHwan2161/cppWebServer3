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

// Get token data from a token vertex
// Returns the concatenated data from all leaf nodes
char* get_token_data(unsigned int vertex_index);

// Add these error code definitions
#define VERTEX_SUCCESS 0
#define VERTEX_ERROR_GARBAGE 1
#define VERTEX_ERROR_IN_GARBAGE_cycle 2

// Create a new token vertex by combining two existing tokens
// Returns the new vertex index or -1 on error
int create_token_vertex(unsigned int first_vertex, unsigned int second_vertex);

#endif // vertex_H 