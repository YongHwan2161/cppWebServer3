#ifndef cycle_H
#define cycle_H

#include <stdbool.h>
#include "../../CGDB.h"

#define SUCCESS 0
#define ERROR -1

#define MAX_TOKEN_LENGTH 100

// Structure to store cycle information
typedef struct {
    uint* vertices;         // Array of vertices in cycle
    ushort* channels;    // Array of channels in cycle
    int count;          // Number of vertices in cycle
} cycleInfo;

// Check if there is a cycle starting from given vertex/channel/axis
bool has_cycle(unsigned int vertex_index, ushort channel_index, ushort axis_number);

// Get detailed information about the cycle
cycleInfo* get_cycle_info(unsigned int vertex_index, ushort channel_index, ushort axis_number);

// Free cycle info structure
void free_cycle_info(cycleInfo* info);

// Add this function declaration
bool is_in_garbage_cycle(unsigned int vertex_index);

// Add this function declaration
int create_cycle(uint* vertices, ushort* channels, int count, ushort axis_number);

// Create a sentence cycle from an array of token vertices
// Returns LINK_SUCCESS or LINK_ERROR
int create_sentence_cycle(uint* token_vertices, int count);

// Get sentence data starting from given vertex/channel
// Returns allocated string containing sentence data or NULL on error
char* get_sentence_data(uint vertex_index, ushort channel_index);
int handle_get_sentence(char* args);
int handle_create_sentence(char* args);
int handle_create_cycle(char* args);
bool validate_cycle(int vertex_index, int channel_index, int axis_number);
int handle_validate_cycle(char* args);
int handle_print_cycle(char* args);

// Create a sentence from an ASCII string
// Returns SUCCESS or ERROR
int handle_create_sentence_from_string(char* args);

#endif // cycle_H 