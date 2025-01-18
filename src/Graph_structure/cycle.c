#include "cycle.h"
#include "../memory.h"
#include "../map.h"
#include "axis.h"
#include "channel.h"
#include "link.h"
#include <stdlib.h>

#define MAX_cycle_vertices 1000

typedef struct {
    uint vertex;
    ushort channel;
    ushort axis;
} Pathvertex;

bool has_cycle(unsigned int vertex_index, ushort channel_index, ushort axis_number) {
    Pathvertex* visited = malloc(MAX_cycle_vertices * sizeof(Pathvertex));
    int visited_count = 0;
    bool has_cycle = false;
    
    // Add starting point
    visited[visited_count].vertex = vertex_index;
    visited[visited_count].channel = channel_index;
    visited[visited_count].axis = axis_number;
    visited_count++;
    
    uint current_vertex = vertex_index;
    ushort current_channel = channel_index;
    ushort current_axis = axis_number;
    
    while (visited_count < MAX_cycle_vertices) {
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
                has_cycle = true;
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
    return has_cycle;
}

cycleInfo* get_cycle_info(unsigned int vertex_index, ushort channel_index, ushort axis_number) {
    Pathvertex* visited = malloc(MAX_cycle_vertices * sizeof(Pathvertex));
    cycleInfo* info = malloc(sizeof(cycleInfo));
    info->vertices = malloc(MAX_cycle_vertices * sizeof(uint));
    info->channels = malloc(MAX_cycle_vertices * sizeof(ushort));
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
    
    while (visited_count < MAX_cycle_vertices) {
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
                // Found cycle - collect vertices in cycle
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

void free_cycle_info(cycleInfo* info) {
    if (info) {
        free(info->vertices);
        free(info->channels);
        free(info);
    }
}

bool is_in_garbage_cycle(unsigned int vertex_index) {
    // Start from garbage vertex (0)
    cycleInfo* info = get_cycle_info(GarbagevertexIndex, 0, 0);
    bool found = false;
    
    // Check if vertex_index exists in the cycle
    for (int i = 0; i < info->count; i++) {
        if (info->vertices[i] == vertex_index) {
            found = true;
            break;
        }
    }
    
    free_cycle_info(info);
    return found;
}

int create_cycle(uint* vertices, ushort* channels, int count, ushort axis_number) {
    if (!vertices || !channels || count < 2) {
        return LINK_ERROR;
    }

    // Create links between consecutive vertices
    for (int i = 0; i < count - 1; i++) {
        if (create_link(vertices[i], channels[i], 
                       vertices[i + 1], channels[i + 1], 
                       axis_number) != LINK_SUCCESS) {
            return LINK_ERROR;
        }
    }

    // Create final link to close the cycle
    if (create_link(vertices[count - 1], channels[count - 1],
                   vertices[0], channels[0],
                   axis_number) != LINK_SUCCESS) {
        return LINK_ERROR;
    }

    return LINK_SUCCESS;
}

int create_sentence_cycle(uint* token_vertices, int count) {
    if (!token_vertices || count < 2) {
        printf("Error: Invalid token array or count\n");
        return LINK_ERROR;
    }

    // Create channels and store channel indices
    ushort* channels = malloc(count * sizeof(ushort));
    if (!channels) {
        printf("Error: Failed to allocate channel array\n");
        return LINK_ERROR;
    }

    // Create new channel for each token vertex
    for (int i = 0; i < count; i++) {
        if (create_channel(token_vertices[i]) != CHANNEL_SUCCESS) {
            printf("Error: Failed to create channel for vertex %u\n", token_vertices[i]);
            free(channels);
            return LINK_ERROR;
        }
        // Get the index of newly created channel
        uint vertex_position = get_vertex_position(token_vertices[i]);
        channels[i] = get_channel_count(Core[vertex_position]) - 1;
    }

    // Create sentence cycle using the new channels
    int result = create_cycle(token_vertices, channels, count, SENTENCE_AXIS);
    free(channels);
    
    return result;
} 