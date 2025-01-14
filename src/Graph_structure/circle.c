#include "circle.h"
#include "../memory.h"
#include "../map.h"
#include "axis.h"
#include "channel.h"
#include <stdlib.h>

#define MAX_CIRCLE_vertices 1000

typedef struct {
    uint vertex;
    ushort channel;
    ushort axis;
} Pathvertex;

bool has_circle(unsigned int vertex_index, ushort channel_index, ushort axis_number) {
    Pathvertex* visited = malloc(MAX_CIRCLE_vertices * sizeof(Pathvertex));
    int visited_count = 0;
    bool has_circle = false;
    
    // Add starting point
    visited[visited_count].vertex = vertex_index;
    visited[visited_count].channel = channel_index;
    visited[visited_count].axis = axis_number;
    visited_count++;
    
    uint current_vertex = vertex_index;
    ushort current_channel = channel_index;
    ushort current_axis = axis_number;
    
    while (visited_count < MAX_CIRCLE_vertices) {
        uint vertex_position = get_vertex_position(current_vertex);
        if (!Core[vertex_position]) break;
        
        uint channel_offset = get_channel_offset(Core[vertex_position], current_channel);
        uint axis_offset = get_axis_offset(Core[vertex_position], current_channel, current_axis);
        
        ushort link_count = *(ushort*)(Core[vertex_position] + channel_offset + axis_offset);
        if (link_count == 0) break;
        
        int link_offset = channel_offset + axis_offset + 2;
        uint next_vertex = *(uint*)(Core[vertex_position] + link_offset);
        ushort next_channel = *(ushort*)(Core[vertex_position] + link_offset + 4);
        
        for (int i = 0; i < visited_count; i++) {
            if (visited[i].vertex == next_vertex && 
                visited[i].channel == next_channel && 
                visited[i].axis == current_axis) {
                has_circle = true;
                goto cleanup;
            }
        }
        
        visited[visited_count].vertex = next_vertex;
        visited[visited_count].channel = next_channel;
        visited[visited_count].axis = current_axis;
        visited_count++;
        
        current_vertex = next_vertex;
        current_channel = next_channel;
    }
    
cleanup:
    free(visited);
    return has_circle;
}

CircleInfo* get_circle_info(unsigned int vertex_index, ushort channel_index, ushort axis_number) {
    Pathvertex* visited = malloc(MAX_CIRCLE_vertices * sizeof(Pathvertex));
    CircleInfo* info = malloc(sizeof(CircleInfo));
    info->vertices = malloc(MAX_CIRCLE_vertices * sizeof(uint));
    info->channels = malloc(MAX_CIRCLE_vertices * sizeof(ushort));
    info->count = 0;
    
    int visited_count = 0;
    
    // Add starting point
    visited[visited_count].vertex = vertex_index;
    visited[visited_count].channel = channel_index;
    visited[visited_count].axis = axis_number;
    visited_count++;
    
    uint current_vertex = vertex_index;
    ushort current_channel = channel_index;
    ushort current_axis = axis_number;
    
    while (visited_count < MAX_CIRCLE_vertices) {
        // printf("visited_count: %d\n", visited_count);
        uint vertex_position = get_vertex_position(current_vertex);
        if (!Core[vertex_position]) break;
        
        uint channel_offset = get_channel_offset(Core[vertex_position], current_channel);
        uint axis_offset = get_axis_offset(Core[vertex_position], current_channel, current_axis);
        
        ushort link_count = *(ushort*)(Core[vertex_position] + channel_offset + axis_offset);
        if (link_count == 0) break;
        
        int link_offset = channel_offset + axis_offset + 2;
        uint next_vertex = *(uint*)(Core[vertex_position] + link_offset);
        ushort next_channel = *(ushort*)(Core[vertex_position] + link_offset + 4);
        
        for (int i = 0; i < visited_count; i++) {
            if (visited[i].vertex == next_vertex && 
                visited[i].channel == next_channel && 
                visited[i].axis == current_axis) {
                // Found circle - collect vertices in circle
                info->count = visited_count - i;
                for (int j = 0; j < info->count; j++) {
                    info->vertices[j] = visited[i + j].vertex;
                    info->channels[j] = visited[i + j].channel;
                }
                goto cleanup;
            }
        }
        
        visited[visited_count].vertex = next_vertex;
        visited[visited_count].channel = next_channel;
        visited[visited_count].axis = current_axis;
        visited_count++;
        
        current_vertex = next_vertex;
        current_channel = next_channel;
    }
    
cleanup:
    free(visited);
    return info;
}

void free_circle_info(CircleInfo* info) {
    if (info) {
        free(info->vertices);
        free(info->channels);
        free(info);
    }
}

bool is_in_garbage_circle(unsigned int vertex_index) {
    // Start from garbage vertex (0)
    CircleInfo* info = get_circle_info(GarbagevertexIndex, 0, 0);
    bool found = false;
    
    // Check if vertex_index exists in the circle
    for (int i = 0; i < info->count; i++) {
        if (info->vertices[i] == vertex_index) {
            found = true;
            break;
        }
    }
    
    free_circle_info(info);
    return found;
} 