#ifndef vertex_H
#define vertex_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// Structure to hold search result
typedef struct {
    unsigned int vertex_index;    // Found vertex index
    char* token_data;     // Token data at vertex
    int matched_length;   // Length of matched data
} TokenSearchResult;

// Function declarations
bool validate_vertex(unsigned int vertex_index);
bool save_vertex_to_file(unsigned int vertex_index);
void create_new_vertex();
int handle_create_vertex(char* args);
int delete_vertex(unsigned int vertex_index);
int handle_delete_vertex(char* args);

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

int handle_create_token(char* args);
int handle_get_token_data(char* args);
int handle_search_token(char* args);
TokenSearchResult* search_token(const char* data, size_t length);
void free_search_result(TokenSearchResult* result);

#endif // vertex_H 