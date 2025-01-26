#include <stdbool.h>
#include "token.h"
#include "node.h"
#include "../vertex.h"
#include "../cycle.h"
#include "../channel.h"
#include "../link.h"
#include "../../memory.h"
#include "../../map.h"
#include "../../cli/command_handler.h"
#include "../../../CGDB.h"

int integrate_token(unsigned int node_index, unsigned int next_node, ushort to_integrate_ch[], ushort channel_count)
{
    int new_node = create_token_node(node_index, next_node);
    if (new_node == -1) return ERROR;
    for (int i = 0; i < channel_count; i++) {
        if (to_integrate_ch[i] == 1) {
            cycleInfo *existing_cycle = get_cycle_info(node_index, i, STRING_AXIS);
            if (existing_cycle == NULL) {
                free_cycle_info(existing_cycle);
                printf("Error: Failed to get cycle info\n");
                return ERROR;
            }
            if (existing_cycle->count <= 1) {
                free_cycle_info(existing_cycle);
                // printf("Error: Cycle count is less than 1\n");
                return ERROR;
            }
            if (existing_cycle->count == 2)
            {
                Vertex start_vertex = (Vertex){existing_cycle->vertices[0], existing_cycle->channels[0]};
                if (create_channel(new_node) != CHANNEL_SUCCESS) {
                    free_cycle_info(existing_cycle);
                    printf("Error: Failed to create channel\n");
                    return ERROR;
                }
                ushort channel_count = get_channel_count(Core[new_node]);
                // if (is_root_vertex(start_vertex)) {
                //     if (CurrentVertex.node == RootVertex.node && CurrentVertex.channel == RootVertex.channel) {
                //         RootVertex = (Vertex){new_node, channel_count - 1};
                //         move_current_vertex((Vertex){new_node, channel_count - 1});
                //     }
                // }
                if (is_start_string_vertex(start_vertex))
                {
                    migrate_parent_vertices(start_vertex, (Vertex){new_node, channel_count - 1});
                    migrate_child_vertices(start_vertex, (Vertex){new_node, channel_count - 1});
                    // clear_channel(start_vertex.node, start_vertex.channel);
                }
                clear_cycle(existing_cycle);
                create_loop(new_node, channel_count - 1, STRING_AXIS);
            } 
            else
            {
                Vertex start_vertex = (Vertex){existing_cycle->vertices[0], existing_cycle->channels[0]};
                if (create_channel(new_node) != CHANNEL_SUCCESS) {
                    printf("Error: Failed to create channel\n");
                    free_cycle_info(existing_cycle);
                    return ERROR;
                }
                ushort channel_count = get_channel_count(Core[new_node]);
                // if (is_root_vertex(start_vertex)) {
                //     if (CurrentVertex.node == RootVertex.node && CurrentVertex.channel == RootVertex.channel) {
                //         RootVertex = (Vertex){new_node, channel_count - 1};
                //         move_current_vertex((Vertex){new_node, channel_count - 1});
                //     }
                // }
                Vertex new_vertex = (Vertex){new_node, channel_count - 1};
                // Vertex start_vertex = (Vertex){existing_cycle->vertices[0], existing_cycle->channels[0]};
                if (is_start_string_vertex(start_vertex))
                {
                    migrate_parent_vertices(start_vertex, new_vertex);
                    migrate_child_vertices(start_vertex, new_vertex);
                }
                replace_new_token(new_vertex, start_vertex, STRING_AXIS);
                clear_channel(start_vertex.node, start_vertex.channel);
            }
            free_cycle_info(existing_cycle);
        }
    }
    return SUCCESS;
}

int integrate_token_prepare(unsigned int node_index)
{
    // Check if node has enough channels for integration
    long node_position = get_node_position(node_index);
    if (node_position == -1) return ERROR;
    ushort channel_count = get_channel_count(Core[node_position]);
    if (channel_count <= 1) {
        return SUCCESS;
    }
    // int new_channel_index = 1;
    // uint new_node = 0;
    uint visited_nodes[channel_count];
    for (int i = 0; i < channel_count; i++) {
        visited_nodes[i] = 0;
    }
    ushort to_integrate_ch[channel_count];
    Vertex next_vertex;
    for (int i = 1; i < channel_count - 1; i++) {
        bool need_integrate = false;
        for (int i = 0; i < channel_count; i++)
        {
            to_integrate_ch[i] = 0;
        }
        if (visited_nodes[i] == 1) continue;
        next_vertex = get_next_vertex_check(node_index, i, STRING_AXIS, 0);
        if (next_vertex.node == 0 && next_vertex.channel == 0) continue;
        to_integrate_ch[i] = 1;

        for (int j = i + 1; j < channel_count; j++) {
            Vertex next_vertex2 = get_next_vertex_check(node_index, j, STRING_AXIS, 0);
            if (next_vertex2.node == 0 && next_vertex2.channel == 0) continue;
            if (are_vertices_in_same_cycle(node_index, i, node_index, j, STRING_AXIS)) continue;
            if (next_vertex.node == next_vertex2.node) {
                // Create combined token
                need_integrate = true;
                visited_nodes[j] = 1;
                to_integrate_ch[j] = 1;
            }
        }
        if (need_integrate)
        {
            integrate_token(node_index, next_vertex.node, to_integrate_ch, channel_count);
        }
    }
    return SUCCESS;
}

int handle_integrate_tokens(char* args) {
    int node_index;
    
    // Parse arguments
    int parsed = sscanf(args, "%d", &node_index);
    if (parsed != 1) {
        print_argument_error("integrate-tokens", "<node_index>", false);
        return ERROR;
    }
    
    // Validate input
    if (!validate_node(node_index)) {
        return ERROR;
    }
    
    // Integrate tokens
    int result = integrate_token_prepare(node_index);
    if (result == SUCCESS) {
        printf("Successfully integrated tokens in node %d\n", node_index);
        return SUCCESS;
    } else {
        printf("Failed to integrate tokens in node %d\n", node_index);
        return ERROR;
    }
}