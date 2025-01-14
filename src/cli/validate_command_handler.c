#include "validate_command_handler.h"
#include "../Graph_structure/axis.h"
#include "../Graph_structure/channel.h"
#include "../Graph_structure/link.h"
#include "../free_space.h"
#include "../memory.h"
#include "command_handler.h"
#include "../tests/axis_tests.h"
#include "../map.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_VISITED_NODES 1000

typedef struct {
    uint node;
    ushort channel;
    ushort axis;
} PathNode;

int validate_free_offsets(char* args) {
    if (args) {
        print_argument_error("validate-free-offsets", "", false);
        return CMD_ERROR;
    }
    
    int failed = test_free_block_offsets();
    return (failed == 0) ? CMD_SUCCESS : CMD_ERROR;
}

bool validate_circle(int node_index, int channel_index, int axis_number) {
    PathNode* visited = malloc(MAX_VISITED_NODES * sizeof(PathNode));
    int visited_count = 0;
    bool has_circle = false;
    
    // Add starting point
    visited[visited_count].node = node_index;
    visited[visited_count].channel = channel_index;
    visited[visited_count].axis = axis_number;
    visited_count++;
    
    uint current_node = node_index;
    ushort current_channel = channel_index;
    ushort current_axis = axis_number;
    
    while (visited_count < MAX_VISITED_NODES) {
        // Get the links for current position
        uint node_position = get_node_position(current_node);
        if (!Core[node_position]) break;
        
        uint channel_offset = get_channel_offset(Core[node_position], current_channel);
        uint axis_offset = get_axis_offset(Core[node_position], current_channel, current_axis);
        // Get link count
        ushort link_count = *(ushort*)(Core[node_position] + channel_offset + axis_offset);
        if (link_count == 0) break;
        
        // Get first link (assuming we follow first link in circle)
        int link_offset = channel_offset + axis_offset + 2; // Skip link count
        uint next_node = *(uint*)(Core[node_position] + link_offset);
        ushort next_channel = *(ushort*)(Core[node_position] + link_offset + 4);
        // Check if we've found a circle
        for (int i = 0; i < visited_count; i++) {
            if (visited[i].node == next_node && 
                visited[i].channel == next_channel && 
                visited[i].axis == current_axis) {
                has_circle = true;
                goto cleanup;
            }
        }
        
        // Add new position to visited
        visited[visited_count].node = next_node;
        visited[visited_count].channel = next_channel;
        visited[visited_count].axis = current_axis;
        visited_count++;
        
        // Move to next position
        current_node = next_node;
        current_channel = next_channel;
    }
    
cleanup:
    free(visited);
    return has_circle;
}
