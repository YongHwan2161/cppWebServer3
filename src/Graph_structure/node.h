#ifndef node_H
#define node_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// Add these error code definitions
#define node_SUCCESS 0
#define node_ERROR_GARBAGE 1
#define node_ERROR_IN_GARBAGE_cycle 2

// Structure to hold search result
typedef struct {
    unsigned int node_index;    // Found node index
    char* token_data;     // Token data at node
    int matched_length;   // Length of matched data
} TokenSearchResult;

// Function declarations
bool validate_node(unsigned int node_index);
bool save_node_to_file(unsigned int node_index);
void create_new_node();
int handle_create_node(char* args);
int delete_node(unsigned int node_index);
int handle_delete_node(char* args);

// Get token data from a token node
// Returns the concatenated data from all leaf nodes
char* get_token_data(unsigned int node_index);


// Create a new token node by combining two existing tokens
// Returns the new node index or -1 on error
int create_token_node(unsigned int first_node, unsigned int second_node);

int handle_create_token(char* args);
int handle_get_token_data(char* args);
int handle_search_token(char* args);
TokenSearchResult* search_token(const char* data, size_t length);
void free_search_result(TokenSearchResult* result);
int handle_integrate_tokens(char* args);

#endif // node_H 