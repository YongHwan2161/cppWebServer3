#include <stdlib.h>
#include <locale.h>
#include "cycle.h"
#include "axis.h"
#include "channel.h"
#include "link.h"
#include "node/node.h"
#include "node/token.h"
#include "../memory.h"
#include "../map.h"
#include "../cli/command_handler.h"
#include "../data_structures/array.h"
#include "../free_space.h"
#include "../../CGDB.h"

#define MAX_cycle_vertices 1000

typedef struct {
    uint node;
    ushort channel;
    ushort axis;
} Pathnode;

bool has_cycle(unsigned int node_index, ushort channel_index, ushort axis_number) {
    Pathnode* visited = (Pathnode*)malloc(MAX_cycle_vertices * sizeof(Pathnode));
    int visited_count = 0;
    bool has_cycle = false;
    
    // Add starting point
    visited[visited_count].node = node_index;
    visited[visited_count].channel = channel_index;
    visited[visited_count].axis = axis_number;
    visited_count++;
    
    uint current_node = node_index;
    ushort current_channel = channel_index;
    ushort current_axis = axis_number;
    
    while (visited_count < MAX_cycle_vertices) {
        long node_position = get_node_position(current_node);
        if (node_position == -1) break;
        
        uint channel_offset = get_channel_offset(Core[(unsigned int)node_position], current_channel);
        uint axis_offset = get_axis_offset(Core[(unsigned int)  node_position], current_channel, current_axis);
        
        ushort link_count = *(ushort*)(Core[node_position] + channel_offset + axis_offset);
        if (link_count == 0) break;
        
        int link_offset = channel_offset + axis_offset + 2;
        uint next_node = *(uint*)(Core[node_position] + link_offset);
        ushort next_channel = *(ushort*)(Core[node_position] + link_offset + 4);
        
        for (int i = 0; i < visited_count; i++) {
            if (visited[i].node == next_node && 
                visited[i].channel == next_channel && 
                visited[i].axis == current_axis) {
                has_cycle = true;
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
    return has_cycle;
}

cycleInfo* get_cycle_info(unsigned int node_index, ushort channel_index, ushort axis_number) {
    Pathnode* visited = (Pathnode*)malloc(MAX_cycle_vertices * sizeof(Pathnode));
    cycleInfo* info = (cycleInfo*)malloc(sizeof(cycleInfo));
    info->vertices = (uint*)malloc(MAX_cycle_vertices * sizeof(uint));
    info->channels = (ushort*)malloc(MAX_cycle_vertices * sizeof(ushort));
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
    
    while (visited_count < MAX_cycle_vertices) {
        // printf("visited_count: %d\n", visited_count);
        long node_position = get_node_position(current_node);
        if (node_position == -1) break;
        
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
                // Found cycle - collect vertices in cycle
                info->count = visited_count - i;
                for (int j = 0; j < info->count; j++) {
                    info->vertices[j] = visited[i + j].node;
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

void free_cycle_info(cycleInfo* info) {
    if (info) {
        free(info->vertices);
        free(info->channels);
        free(info);
    }
}

bool is_in_garbage_cycle(unsigned int node_index) {
    // Start from garbage node (0)
    cycleInfo* info = get_cycle_info(GarbagenodeIndex, 0, 0);
    bool found = false;
    
    // Check if node_index exists in the cycle
    for (int i = 0; i < info->count; i++) {
        if (info->vertices[i] == node_index) {
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
int create_string_cycle(uint* token_vertices, ushort* channels, int count) {
    if (!token_vertices) {
        printf("Error: Invalid token array or count\n");
        return LINK_ERROR;
    }
    // Create string cycle using the new channels
    int result = create_cycle(token_vertices, channels, count, STRING_AXIS);
    
    return result;
}

char* get_string_data(uint node_index, ushort channel_index) {
    // Get cycle info
    cycleInfo* info = get_cycle_info(node_index, channel_index, STRING_AXIS);
    if (!info || info->count < 1) {
        printf("Error: No valid string cycle found\n");
        free_cycle_info(info);
        return NULL;
    }

    // Allocate buffer for string data
    char* string = (char*)malloc(8192);  // Add explicit cast to char*
    if (!string) {
        free_cycle_info(info);
        return NULL;
    }
    int string_len = 0;

    // Get token data for each node in cycle
    for (int i = 0; i < info->count; i++) {
        char* token_data = get_token_data(info->vertices[i]);
        if (!token_data) {
            printf("Error: Failed to get token data from node %u\n", info->vertices[i]);
            free(string);
            free_cycle_info(info);
            return NULL;
        }

        // Add token data to string
        int token_len = strlen(token_data);
        if (string_len + token_len >= 1023) {
            printf("Error: string too long\n");
            free(token_data);
            free(string);
            free_cycle_info(info);
            return NULL;
        }

        // Copy token data without adding spaces
        memcpy(string + string_len, token_data, token_len);
        string_len += token_len;

        free(token_data);
    }

    string[string_len] = '\0';
    free_cycle_info(info);
    return string;
} 
#define MAX_string_TOKENS 500
int clear_cycle(cycleInfo* info) {
    clear_channels(info->vertices, info->channels, info->count);
    return LINK_SUCCESS;
}
int handle_create_string(char* args, uint* start_node, ushort* start_channel, bool is_root, bool is_embedded) {
    if (!args || !*args) {
        print_argument_error("create-string", "<text>", false);
        return ERROR;
    }

    uint tokens[MAX_string_TOKENS];
    ushort channels[MAX_string_TOKENS];
    int count = 0;
    const char* current_pos = args;
    size_t remaining_len = strlen(args);

    TokenSearchResult *result_first = search_token(current_pos, remaining_len);
    if (!result_first) 
    {
        printf("Error: Failed to search token\n");
        return ERROR; 
    }

    if (remaining_len == (size_t)result_first->matched_length)
    { // if the remaining length is 0, then we are at the end of the string
        tokens[0] = result_first->node_index;
        channels[0] = (ushort)recycle_or_create_channel(result_first->node_index);
        if (channels[0] == (ushort)CHANNEL_ERROR)
        {
            printf("Error: Failed to create channel for node %u\n", result_first->node_index);
            free_search_result(result_first);
            return ERROR;
        }

        if (create_loop(tokens[0], channels[0], 2) != LINK_SUCCESS)
        {
            printf("Error: Failed to create loop for node %u\n", result_first->node_index);
            free_search_result(result_first);
            return ERROR;
        } else {
            create_property(tokens[0], channels[0], STRING_START_NODE);
            Vertex start_vertex = {tokens[0], channels[0]};
            if (!is_root && !is_embedded)
            {
                create_bidirectional_link(CurrentVertex, start_vertex);
                // optimize_string_cycle(tokens, count, save);
            }
        }
        *start_node = tokens[0];
        *start_channel = channels[0];
        free_search_result(result_first);

        return SUCCESS;
    }
    // bool need_search = true;
    // Tokenize input using search_token
    while (remaining_len > 0 && count < MAX_string_TOKENS) {
        TokenSearchResult *result = search_token(current_pos, remaining_len);
        if (!result) break;
        
        tokens[count] = result->node_index;
        if (tokens[count] == tokens[count-1]) // repeat token
        {
            create_channel(tokens[count]);
            channels[count] = channels[count-1] + 1;
        }
        else // ordinary token sequence
        {
            channels[count] = recycle_or_create_channel(result->node_index);
        }
        if (channels[count] == (ushort)CHANNEL_ERROR) {
            printf("Error: Failed to create channel for node %u\n", result->node_index);
            free_search_result(result);
            return ERROR;
        }
        if (count > 0)
        {
            create_link(tokens[count - 1], channels[count - 1], tokens[count], channels[count], 2); // create a link between the previous token and the current token
        }

        // update the current position and remaining length
        current_pos += result->matched_length;
        remaining_len -= result->matched_length;
        count++;
        free_search_result(result);
    }

    // Set start node and channel before any potential error returns
    *start_node = tokens[0];
    *start_channel = channels[0];

    if (create_link(tokens[count - 1], channels[count - 1], tokens[0], channels[0], 2) != LINK_SUCCESS) {
        printf("Error: Failed to create link between the last token and the first token\n");
        return ERROR;
    } else {
        create_property(tokens[0], channels[0], STRING_START_NODE);
        Vertex start_vertex = {tokens[0], channels[0]};
        if (!is_root){
            if (!is_embedded)
            {
                create_bidirectional_link(CurrentVertex, start_vertex);
            }
            count = remove_duplicates(tokens, count);
            optimize_string_cycle(tokens, count);
        }
        // printf("Successfully created link between the last token and the first token\n");
        return SUCCESS;
    }
}
int optimize_string_cycle(uint* nodes, int count) {
    for (int i = 0; i < count; i++) {
        if (integrate_token_prepare(nodes[i]) != SUCCESS) {
            printf("Error: Failed to integrate token data in node %u\n", nodes[i]);
            return CMD_ERROR;
        }
    }
    return CMD_SUCCESS;
}
int handle_optimize_string(char* args) {
    uint vertices[MAX_cycle_vertices];
    ushort channel = 0;
    ushort axis_number = 0;
    // int count = 0;
    if (sscanf(args, "%u %hu %hu", &vertices[0], &channel, &axis_number) != 3) {
        print_argument_error("optimize-string", "<node> <channel> <axis>", false);
        return CMD_ERROR;
    }
    cycleInfo* info = get_cycle_info(vertices[0], channel, axis_number);
    if (!info) {
        printf("Error: No cycle found at node %u channel %hu axis %hu\n", 
               vertices[0], channel, axis_number);
        return CMD_ERROR;
    }
    if (optimize_string_cycle(info->vertices, info->count) == CMD_SUCCESS) {
        printf("Successfully optimized string cycle\n");
        free_cycle_info(info);
        return CMD_SUCCESS;
    } else {
        printf("No optimizations possible or error occurred\n");
        free_cycle_info(info);
        return CMD_ERROR;
    }
}
int handle_get_string(char* args) {
    // Set locale to support UTF-8
    setlocale(LC_ALL, "en_US.UTF-8");
    
    uint node_index;
    ushort channel_index;
    
    // Parse arguments
    if (sscanf(args, "%u %hu", &node_index, &channel_index) != 2) {
        print_argument_error("get-string", "<node_index> <channel_index>", false);
        return CMD_ERROR;
    }
    
    // Get string data
    char* data = get_string_data(node_index, channel_index);
    if (!data) {
        printf("Error: Failed to get string data\n");
        return CMD_ERROR;
    }
    
    // Print the data in multiple formats
    printf("string data starting from node %u, channel %u:\n", node_index, channel_index);
    
    // Print raw data as stored
    printf("Raw: %s\n", data);
    
    // Print hexadecimal format
    printf("HEX: ");
    for (size_t i = 0; data[i] != '\0'; i++) {
        printf("%02X ", (unsigned char)data[i]);
    }
    printf("\n");

    // Print UTF-8 format with proper handling
    printf("UTF-8: ");
    fflush(stdout);  // Ensure proper output ordering
    
    // Write the entire string at once for proper UTF-8 handling
    fwrite(data, 1, strlen(data), stdout);
    printf("\n");
    
    // Clean up
    free(data);
    return CMD_SUCCESS;
}
int handle_create_cycle(char* args) {
    // Maximum vertices in a cycle for command parsing
    #define MAX_CMD_CYCLE_VERTICES 100
    
    uint vertices[MAX_CMD_CYCLE_VERTICES];
    ushort channels[MAX_CMD_CYCLE_VERTICES];
    int count = 0;
    int axis_number;
    
    // Parse the axis number first (it's the last argument)
    char* last_space = strrchr(args, ' ');
    if (!last_space) {
        print_argument_error("create-cycle", "<node1> <ch1> <node2> <ch2> ... <axis>", false);
        return CMD_ERROR;
    }
    
    // Parse axis number
    if (sscanf(last_space + 1, "%d", &axis_number) != 1) {
        printf("Error: Invalid axis number\n");
        return CMD_ERROR;
    }
    
    // Null terminate the node/channel list for parsing
    *last_space = '\0';
    
    // Parse node and channel pairs
    char* token = strtok(args, " ");
    while (token && count < MAX_CMD_CYCLE_VERTICES) {
        // Parse node
        if (sscanf(token, "%u", &vertices[count]) != 1) {
            printf("Error: Invalid node index\n");
            return CMD_ERROR;
        }
        
        // Get next token for channel
        token = strtok(NULL, " ");
        if (!token) {
            printf("Error: Missing channel for node %u\n", vertices[count]);
            return CMD_ERROR;
        }
        
        // Parse channel
        if (sscanf(token, "%hu", &channels[count]) != 1) {
            printf("Error: Invalid channel index\n");
            return CMD_ERROR;
        }
        
        count++;
        token = strtok(NULL, " ");
    }
    
    if (count < 2) {
        printf("Error: At least 2 vertices are required to create a cycle\n");
        return CMD_ERROR;
    }

    // Check if any node in the proposed cycle already has a cycle
    for (int i = 0; i < count; i++) {
        if (has_cycle(vertices[i], channels[i], axis_number)) {
            printf("Error: node %u channel %u already belongs to a cycle on axis %d\n", 
                   vertices[i], channels[i], axis_number);
            return CMD_ERROR;
        }
    }
    
    // Create the cycle
    int result = create_cycle(vertices, channels, count, axis_number);
    if (result == LINK_SUCCESS) {
        printf("Successfully created cycle with %d vertices\n", count);
        return CMD_SUCCESS;
    } else {
        printf("Failed to create cycle\n");
        return CMD_ERROR;
    }
}

bool validate_cycle(int node_index, int channel_index, int axis_number) {
    cycleInfo* info = get_cycle_info(node_index, channel_index, axis_number);
    bool has_cycle = info->count > 0;
    
    if (has_cycle) {
        printf("Found cycle with %d vertices:\n", info->count);
        for (int i = 0; i < info->count; i++) {
            printf("node %u, Channel %u\n", info->vertices[i], info->channels[i]);
        }
    }
    
    free_cycle_info(info);
    return has_cycle;
}

int handle_validate_cycle(char* args) {
    int node_index, channel_index, axis_number;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d %d", &node_index, &channel_index, &axis_number);
    if (parsed != 3) {
        print_argument_error("validate-cycle", "<node_index> <channel_index> <axis_number>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (!validate_node(node_index)) {
        printf("Error: node %d does not exist\n", node_index);
        return CMD_ERROR;
    }
    
    bool has_cycle = validate_cycle(node_index, channel_index, axis_number);
    printf("Path from node %d, channel %d, axis %d %s a cycle\n",
           node_index, channel_index, axis_number,
           has_cycle ? "forms" : "does not form");
           
    return CMD_SUCCESS;
}

int handle_print_cycle(char* args) {
    int node_index, channel_index, axis_number;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d %d", &node_index, &channel_index, &axis_number);
    if (parsed != 3) {
        print_argument_error("print-cycle", "<node_index> <channel_index> <axis_number>", false);
        return CMD_ERROR;
    }
    
    // Get cycle information
    cycleInfo* info = get_cycle_info(node_index, channel_index, axis_number);
    
    if (info->count == 0) {
        printf("No cycle found starting from node %d, channel %d, axis %d\n",
               node_index, channel_index, axis_number);
    } else {
        printf("Found cycle with %d vertices:\n", info->count);
        printf("Path: ");
        for (int i = 0; i < info->count; i++) {
            printf("(node %u, Ch %u)", info->vertices[i], info->channels[i]);
            if (i < info->count - 1) {
                printf(" -> ");
            }
        }
        printf(" -> (node %u, Ch %u)\n", info->vertices[0], info->channels[0]);
    }
    
    free_cycle_info(info);
    return CMD_SUCCESS;
}
int replace_new_token(Vertex new_vertex, Vertex old_vertex, ushort axis_number) {
    cycleInfo* info = get_cycle_info(old_vertex.node, old_vertex.channel, axis_number);
    if (info->count == 0) {
        return CMD_SUCCESS;
    }
    if (info->count == 2) {
        delete_link(info->vertices[0], info->channels[0], info->vertices[1], info->channels[1], axis_number);
        delete_link(info->vertices[1], info->channels[1], info->vertices[0], info->channels[0], axis_number);
        create_loop(new_vertex.node, new_vertex.channel, axis_number);
        return CMD_SUCCESS;
    }

    delete_link(info->vertices[info->count - 1], info->channels[info->count - 1], info->vertices[0], info->channels[0], axis_number);
    delete_link(info->vertices[0], info->channels[0], info->vertices[1], info->channels[1], axis_number);
    delete_link(info->vertices[1], info->channels[1], info->vertices[2], info->channels[2], axis_number);
    create_link(info->vertices[info->count - 1], info->channels[info->count - 1], new_vertex.node, new_vertex.channel, axis_number);
    create_link(new_vertex.node, new_vertex.channel, info->vertices[2], info->channels[2], axis_number);
    return CMD_SUCCESS;
}

// Insert a path into an existing cycle at specified position
int insert_path_into_cycle(uint insert_node, ushort insert_channel, 
                          uint* path_vertices, ushort* path_channels, int path_length,
                          ushort axis_number) {
    if (!path_vertices || !path_channels || path_length < 1) {
        printf("Error: Invalid path data\n");
        return LINK_ERROR;
    }

    // Get cycle info at insertion point
    cycleInfo* cycle = get_cycle_info(insert_node, insert_channel, axis_number);
    if (!cycle || cycle->count < 2) {
        printf("Error: No valid cycle found at insertion point\n");
        if (cycle) free_cycle_info(cycle);
        return LINK_ERROR;
    }

    // Find insertion position in cycle
    int insert_pos = -1;
    for (int i = 0; i < cycle->count; i++) {
        if (cycle->vertices[i] == insert_node && 
            cycle->channels[i] == insert_channel) {
            insert_pos = i;
            break;
        }
    }

    if (insert_pos == -1) {
        printf("Error: Insertion point not found in cycle\n");
        free_cycle_info(cycle);
        return LINK_ERROR;
    }

    // Break cycle at insertion point
    uint next_node = cycle->vertices[(insert_pos + 1) % cycle->count];
    ushort next_channel = cycle->channels[(insert_pos + 1) % cycle->count];
    
    // Remove link at insertion point
    if (delete_link(insert_node, insert_channel, next_node, next_channel, axis_number) != LINK_SUCCESS) {
        printf("Error: Failed to break cycle at insertion point\n");
        free_cycle_info(cycle);
        return LINK_ERROR;
    }

    // Link path start to insertion point
    if (create_link(insert_node, insert_channel,
                   path_vertices[0], path_channels[0],
                   axis_number) != LINK_SUCCESS) {
        printf("Error: Failed to link path start\n");
        free_cycle_info(cycle);
        return LINK_ERROR;
    }

    // Create links along the path
    for (int i = 0; i < path_length - 1; i++) {
        if (create_link(path_vertices[i], path_channels[i],
                       path_vertices[i + 1], path_channels[i + 1],
                       axis_number) != LINK_SUCCESS) {
            printf("Error: Failed to link path vertices\n");
            free_cycle_info(cycle);
            return LINK_ERROR;
        }
    }

    // Link path end to next node in original cycle
    if (create_link(path_vertices[path_length - 1], path_channels[path_length - 1],
                   next_node, next_channel,
                   axis_number) != LINK_SUCCESS) {
        printf("Error: Failed to link path end\n");
        free_cycle_info(cycle);
        return LINK_ERROR;
    }

    free_cycle_info(cycle);
    return LINK_SUCCESS;
}

// Command handler for inserting path into cycle
int handle_insert_path(char* args) {
    uint insert_node;
    ushort insert_channel, axis_number;
    uint path_vertices[MAX_cycle_vertices];
    ushort path_channels[MAX_cycle_vertices];
    int path_length = 0;

    // Parse insertion point and axis
    char* token = strtok(args, " ");
    if (!token || sscanf(token, "%u", &insert_node) != 1) {
        print_argument_error("insert-path", "<node> <channel> <axis> <path_vertices_and_channels...>", false);
        return CMD_ERROR;
    }

    token = strtok(NULL, " ");
    if (!token || sscanf(token, "%hu", &insert_channel) != 1) {
        print_argument_error("insert-path", "<node> <channel> <axis> <path_vertices_and_channels...>", false);
        return CMD_ERROR;
    }

    token = strtok(NULL, " ");
    if (!token || sscanf(token, "%hu", &axis_number) != 1) {
        print_argument_error("insert-path", "<node> <channel> <axis> <path_vertices_and_channels...>", false);
        return CMD_ERROR;
    }

    // Parse path vertices and channels
    while ((token = strtok(NULL, " ")) != NULL && path_length < MAX_cycle_vertices) {
        if (sscanf(token, "%u", &path_vertices[path_length]) != 1) {
            printf("Error: Invalid node index in path\n");
            return CMD_ERROR;
        }

        token = strtok(NULL, " ");
        if (!token || sscanf(token, "%hu", &path_channels[path_length]) != 1) {
            printf("Error: Missing channel for node %u\n", path_vertices[path_length]);
            return CMD_ERROR;
        }

        path_length++;
    }

    if (path_length == 0) {
        printf("Error: No path vertices provided\n");
        return CMD_ERROR;
    }

    // Validate all channels exist
    for (int i = 0; i < path_length; i++) {
        long node_position = get_node_position(path_vertices[i]);
        if (node_position == -1) {
            printf("Error: node %u not loaded\n", path_vertices[i]);
            return CMD_ERROR;
        }
        
        ushort channel_count = get_channel_count(Core[node_position]);
        if (path_channels[i] >= channel_count) {
            printf("Error: Channel %hu does not exist in node %u (max: %hu)\n", 
                   path_channels[i], path_vertices[i], (ushort)(channel_count - 1));
            return CMD_ERROR;
        }
    }

    // Insert path into cycle
    if (insert_path_into_cycle(insert_node, insert_channel,
                              path_vertices, path_channels, path_length,
                              axis_number) == LINK_SUCCESS) {
        printf("Successfully inserted path of length %d into cycle\n", path_length);
        return CMD_SUCCESS;
    }

    printf("Error: Failed to insert path into cycle\n");
    return CMD_ERROR;
}

// Delete a path from an existing cycle
int delete_path_from_cycle(uint start_node, ushort start_channel, 
                          int path_length, ushort axis_number) {
    if (path_length < 1) {
        printf("Error: Invalid path length\n");
        return LINK_ERROR;
    }

    // Get cycle info at deletion point
    cycleInfo* cycle = get_cycle_info(start_node, start_channel, axis_number);
    if (!cycle || cycle->count < path_length + 2) {
        printf("Error: No valid cycle found or cycle too short\n");
        if (cycle) free_cycle_info(cycle);
        return LINK_ERROR;
    }

    // Find start position in cycle
    int start_pos = -1;
    for (int i = 0; i < cycle->count; i++) {
        if (cycle->vertices[i] == start_node && 
            cycle->channels[i] == start_channel) {
            start_pos = i;
            break;
        }
    }

    if (start_pos == -1) {
        printf("Error: Start point not found in cycle\n");
        free_cycle_info(cycle);
        return LINK_ERROR;
    }

    // Get vertices to be removed and end connection points
    uint end_node = cycle->vertices[(path_length - 1) % cycle->count];
    ushort end_channel = cycle->channels[(path_length - 1) % cycle->count];
    uint next_node = cycle->vertices[(path_length) % cycle->count];
    ushort next_channel = cycle->channels[(path_length) % cycle->count];

    // Break connections at path boundaries
    if (delete_link(cycle->vertices[cycle->count - 1], cycle->channels[cycle->count - 1], 
                   cycle->vertices[0], cycle->channels[0],
                   axis_number) != LINK_SUCCESS ||
        delete_link(end_node, end_channel,
                   next_node, next_channel,
                   axis_number) != LINK_SUCCESS) {
        printf("Error: Failed to break cycle at path boundaries\n");
        free_cycle_info(cycle);
        return LINK_ERROR;
    }

    // Create new link to close the cycle
    if (create_link(start_node, start_channel,
                   next_node, next_channel,
                   axis_number) != LINK_SUCCESS) {
        printf("Error: Failed to close cycle\n");
        free_cycle_info(cycle);
        return LINK_ERROR;
    }

    // Clear channels in removed path
    for (int i = 0; i < path_length; i++) {
        int pos = (start_pos + i) % cycle->count;
        if (clear_channel(cycle->vertices[pos], cycle->channels[pos]) != CHANNEL_SUCCESS) {
            printf("Warning: Failed to clear channel in removed path\n");
        }
    }

    free_cycle_info(cycle);
    return LINK_SUCCESS;
}

// Command handler for deleting path from cycle
int handle_delete_path(char* args) {
    uint start_node;
    ushort start_channel, axis_number;
    int path_length;

    // Parse arguments
    if (sscanf(args, "%u %hu %hu %d", &start_node, &start_channel, 
               &axis_number, &path_length) != 4) {
        print_argument_error("delete-path", 
                           "<node> <channel> <axis> <path_length>", false);
        return CMD_ERROR;
    }

    // Delete path from cycle
    if (delete_path_from_cycle(start_node, start_channel,
                              path_length, axis_number) == LINK_SUCCESS) {
        printf("Successfully deleted path of length %d from cycle\n", path_length);
        return CMD_SUCCESS;
    }

    printf("Error: Failed to delete path from cycle\n");
    return CMD_ERROR;
}

// Check if two vertices are in the same cycle
bool are_vertices_in_same_cycle(uint node1, ushort channel1, 
                              uint node2, ushort channel2, 
                              ushort axis_number) {
    cycleInfo* cycle = get_cycle_info(node1, channel1, axis_number);
    if (!cycle) return false;
    
    // Search for second vertex in the cycle
    bool found = false;
    for (int i = 0; i < cycle->count; i++) {
        if (cycle->vertices[i] == node2 && cycle->channels[i] == channel2) {
            found = true;
            break;
        }
    }
    
    free_cycle_info(cycle);
    return found;
}

// Create string from text file content
int create_strings_from_file(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filepath);
        return ERROR;
    }

    char line[8192];  // Increased buffer size for safety
    uint start_node;
    ushort start_channel;
    int result = SUCCESS;
    sync_flag = false;

    while (fgets(line, sizeof(line), file)) {
        // Remove trailing newline if present
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }

        // Skip empty lines
        if (strlen(line) == 0) continue;

        // Create string with embedded flag set to true
        if (handle_create_string(line, &start_node, &start_channel, false, true) != SUCCESS) {
            printf("Error: Failed to create string for line: %s\n", line);
            result = ERROR;
            break;
        }
    }
    sync_flag = true;
    fclose(file);
    save_free_space();
    save_all_nodes();
    save_map_all();
    save_current_node_count();
    return result;
}
// Handle upload-text command
int handle_upload_text(char* args) {
    if (!args || !*args) {
        print_argument_error("upload-text", "<filepath>", true);
        return CMD_ERROR;
    }

    // Remove quotes if present
    if (args[0] == '"') {
        args++;
        size_t len = strlen(args);
        if (len > 0 && args[len-1] == '"') {
            args[len-1] = '\0';
        }
    }

    if (create_strings_from_file(args) == SUCCESS) {
        printf("Successfully uploaded text from %s\n", args);
        return CMD_SUCCESS;
    }

    // save_inconsistent_nodes();
    printf("Error: Failed to upload text from %s\n", args);
    return CMD_ERROR;
}
