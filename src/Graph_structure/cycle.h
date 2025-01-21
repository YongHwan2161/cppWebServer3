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

// Check if there is a cycle starting from given node/channel/axis
bool has_cycle(unsigned int node_index, ushort channel_index, ushort axis_number);

// Get detailed information about the cycle
cycleInfo* get_cycle_info(unsigned int node_index, ushort channel_index, ushort axis_number);

// Free cycle info structure
void free_cycle_info(cycleInfo* info);

// Add this function declaration
bool is_in_garbage_cycle(unsigned int node_index);

// Add this function declaration
int create_cycle(uint* vertices, ushort* channels, int count, ushort axis_number);

// Create a string cycle from an array of token vertices
// Returns LINK_SUCCESS or LINK_ERROR
int create_string_cycle(uint* token_vertices, ushort* channels, int count);

// Get string data starting from given node/channel
// Returns allocated string containing string data or NULL on error
char* get_string_data(uint node_index, ushort channel_index);
int handle_get_string(char* args);
int handle_create_string(char* args, uint* start_node, ushort* start_channel);
int handle_create_cycle(char* args);
bool validate_cycle(int node_index, int channel_index, int axis_number);
int handle_validate_cycle(char* args);
int handle_print_cycle(char* args);
int handle_delete_path(char* args);

// Create a string from an ASCII string
// Returns SUCCESS or ERROR
// int handle_create_string_from_string(char* args);
int handle_insert_path(char* args);

// Insert a path into an existing cycle
int insert_path_into_cycle(uint insert_node, ushort insert_channel,
                          uint* path_vertices, ushort* path_channels, 
                          int path_length, ushort axis_number);

// Delete a path from an existing cycle
int delete_path_from_cycle(uint start_node, ushort start_channel,
                          int path_length, ushort axis_number);

#endif // cycle_H 