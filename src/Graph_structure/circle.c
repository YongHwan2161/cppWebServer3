#include "circle.h"
#include "../memory.h"
#include "../map.h"
#include "axis.h"
#include "channel.h"
#include <stdlib.h>

#define MAX_CIRCLE_NODES 1000

typedef struct {
    uint node;
    ushort channel;
    ushort axis;
} PathNode;

bool has_circle(int node_index, int channel_index, int axis_number) {
    PathNode* visited = malloc(MAX_CIRCLE_NODES * sizeof(PathNode));
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
    
    while (visited_count < MAX_CIRCLE_NODES) {
        uint node_position = get_node_position(current_node);
        if (!Core[node_position]) break;
        
        uint channel_offset = get_channel_offset(Core[node_position], current_channel);
        uint axis_offset = get_axis_offset(Core[node_position], current_channel, current_axis);
        
        ushort link_count = *(ushort*)(Core[node_position] + channel_offset + axis_offset);
        if (link_count == 0) break;
        
        int link_offset = channel_offset + axis_offset + 2;
        uint next_node = *(uint*)(Core[node_position] + link_offset);
        ushort next_channel = *(ushort*)(Core[node_position] + link_offset + 4);
        
        for (int i = 0; i < visited_count; i++) {
            if (visited[i].node == next_node && 
                visited[i].channel == next_channel && 
                visited[i].axis == current_axis) {
                has_circle = true;
                goto cleanup;
            }
        }
        
        visited[visited_count].node = next_node;
        visited[visited_count].channel = next_channel;
        visited[visited_count].axis = current_axis;
        visited_count++;
        
        current_node = next_node;
        current_channel = next_channel;
    }
    
cleanup:
    free(visited);
    return has_circle;
}

CircleInfo* get_circle_info(int node_index, int channel_index, int axis_number) {
    PathNode* visited = malloc(MAX_CIRCLE_NODES * sizeof(PathNode));
    CircleInfo* info = malloc(sizeof(CircleInfo));
    info->nodes = malloc(MAX_CIRCLE_NODES * sizeof(uint));
    info->channels = malloc(MAX_CIRCLE_NODES * sizeof(ushort));
    info->count = 0;
    
    int visited_count = 0;
    
    // Add starting point
    visited[visited_count].node = node_index;
    visited[visited_count].channel = channel_index;
    visited[visited_count].axis = axis_number;
    visited_count++;
    
    uint current_node = node_index;
    ushort current_channel = channel_index;
    ushort current_axis = axis_number;
    
    while (visited_count < MAX_CIRCLE_NODES) {
        uint node_position = get_node_position(current_node);
        if (!Core[node_position]) break;
        
        uint channel_offset = get_channel_offset(Core[node_position], current_channel);
        uint axis_offset = get_axis_offset(Core[node_position], current_channel, current_axis);
        
        ushort link_count = *(ushort*)(Core[node_position] + channel_offset + axis_offset);
        if (link_count == 0) break;
        
        int link_offset = channel_offset + axis_offset + 2;
        uint next_node = *(uint*)(Core[node_position] + link_offset);
        ushort next_channel = *(ushort*)(Core[node_position] + link_offset + 4);
        
        for (int i = 0; i < visited_count; i++) {
            if (visited[i].node == next_node && 
                visited[i].channel == next_channel && 
                visited[i].axis == current_axis) {
                // Found circle - collect nodes in circle
                info->count = visited_count - i;
                for (int j = 0; j < info->count; j++) {
                    info->nodes[j] = visited[i + j].node;
                    info->channels[j] = visited[i + j].channel;
                }
                goto cleanup;
            }
        }
        
        visited[visited_count].node = next_node;
        visited[visited_count].channel = next_channel;
        visited[visited_count].axis = current_axis;
        visited_count++;
        
        current_node = next_node;
        current_channel = next_channel;
    }
    
cleanup:
    free(visited);
    return info;
}

void free_circle_info(CircleInfo* info) {
    if (info) {
        free(info->nodes);
        free(info->channels);
        free(info);
    }
} 