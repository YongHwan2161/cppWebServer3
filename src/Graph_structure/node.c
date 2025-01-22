#include "node.h"
#include "../../CGDB.h"
#include "channel.h"
#include "axis.h"
#include <stdio.h>
#include "link.h"  // For Core array access
#include "cycle.h"
#include "../map.h"
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include "../data_structures/stack.h"
#include "../cli/command_handler.h"

#define MAX_STACK_SIZE 1000

// Initial node values
static uchar initValues[16] = {
    4,  0,     // allocated size power (2^4 = 16 bytes)
    14, 0, 0, 0,  // actual used size (14 bytes initially)
    1,  0,     // number of channels (1)
    12, 0, 0, 0,   // offset for channel 0 (starts at byte 12)
    0,  0,     // number of axes (0)
    0,  0      // remaining bytes initialized to 0
};

/**
 * Validates if a node index is valid and the node exists in Core
 * 
 * @param node_index Index of the node to validate
 * @return true if valid, false if invalid
 */
bool validate_node(unsigned int node_index) {
    // Check if node exists in Core
    uint node_position = get_node_position(node_index);
    if (!Core[node_position]) {
        printf("Error: Invalid node index %u\n", node_index);
        return false;
    }

    return true;
}

/**
 * Saves node data to data.bin file and updates map.bin
 * 
 * @param node_index Index of the node to save
 * @return true if successful, false if failed
 */
bool save_node_to_file(unsigned int node_index) {
    if (!validate_node(node_index)) {
        return false;
    }
    // printf("start save_node_to_file\n");
    uint node_position = CoreMap[node_index].core_position;
    uchar* node = Core[node_position];
    // Try to open data file, create if doesn't exist
    FILE* data_file = fopen(DATA_FILE, "r+b");
    if (!data_file) {
        data_file = fopen(DATA_FILE, "wb");
        if (!data_file) {
            printf("Error: Failed to create data.bin\n");
            return false;
        }
        fclose(data_file);
        data_file = fopen(DATA_FILE, "r+b");
    }
    // printf("file offset: 0x%lx\n", CoreMap[node_index].file_offset);
    // Write node data
    if (fseek(data_file, CoreMap[node_index].file_offset, SEEK_SET) != 0) {
        printf("Error: Failed to seek in data.bin\n");
        fclose(data_file);
        return false;
    }

    size_t node_size = 1 << (*(ushort*)node);
    if (fwrite(node, 1, node_size, data_file) != node_size) {
        printf("Error: Failed to write to data.bin\n");
        fclose(data_file);
        return false;
    }
    fclose(data_file);

    // Try to open map file, create if doesn't exist
    FILE* map_file = fopen(MAP_FILE, "r+b");
    if (!map_file) {
        map_file = fopen(MAP_FILE, "wb");
        if (!map_file) {
            printf("Error: Failed to create map.bin\n");
            return false;
        }
        // Write initial node count
        // uint initial_count = 256;
        // fwrite(&initial_count, sizeof(uint), 1, map_file);
        // // Initialize all offsets to 0
        // long zero_offset = 0;
        // for (int i = 0; i < 256; i++) {
        //     fwrite(&zero_offset, sizeof(long), 1, map_file);
        // }
        fclose(map_file);
        map_file = fopen(MAP_FILE, "r+b");
    }

    // Update map entry
    if (fseek(map_file, sizeof(uint) + (node_index * sizeof(long)), SEEK_SET) != 0) {
        printf("Error: Failed to seek in map.bin\n");
        fclose(map_file);
        return false;
    }

    if (fwrite(&CoreMap[node_index].file_offset, sizeof(long), 1, map_file) != 1) {
        printf("Error: Failed to write to map.bin\n");
        fclose(map_file);
        return false;
    }
    
    fclose(map_file);
    return true;
}

bool save_current_node_count() {
    FILE* map_file = fopen(MAP_FILE, "r+b");
    if (!map_file) {
        printf("Error: Failed to open map.bin in save_current_node_count\n");
        map_file = fopen(MAP_FILE, "wb");
        if (!map_file) {
            printf("Error: Failed to create map.bin in save_current_node_count\n");
            return false;
        }
    }
    fwrite(&CurrentnodeCount, sizeof(uint), 1, map_file);
    fclose(map_file);
    return true;
}
void initialize_node(uchar** node) {
    for (int i = 0; i < 16; ++i) {
        (*node)[i] = initValues[i];
    }
}
void create_new_node() {
    uchar* newnode = (uchar*)malloc(16 * sizeof(uchar));  // Always allocate 16 bytes initially
    // printf("Creating new node at index %d\n", CurrentnodeCount);
    initialize_node(&newnode);
    CurrentnodeCount++;
    save_current_node_count();
    Core[CurrentnodeCount - 1] = newnode;
    CoreSize++;
    CoreMap[CurrentnodeCount - 1].core_position = CurrentnodeCount - 1;
    CoreMap[CurrentnodeCount - 1].is_loaded = 1;
    if (CurrentnodeCount == 1) {
        CoreMap[CurrentnodeCount - 1].file_offset = 0;
    } else {
        uint last_node_size = 1 << (*(ushort*)Core[CurrentnodeCount - 2]);
        // printf("Last node size: %d\n", last_node_size);
        uint file_offset = CoreMap[CurrentnodeCount - 2].file_offset + last_node_size;
        CoreMap[CurrentnodeCount - 1].file_offset = file_offset;
    }
    save_node_to_file(CurrentnodeCount - 1);
    printf("node created at index %d\n", CurrentnodeCount - 1);
}

int handle_create_node(char* args) {
    // No arguments needed, but check if any were provided
    if (args && *args != '\0') {
        print_argument_error("create-node", "", false);
        return CMD_ERROR;
    }
    
    // Check if we've reached the maximum number of vertices
    if (CurrentnodeCount >= MaxCoreSize) {
        printf("Error: Maximum number of vertices (%d) reached\n", MaxCoreSize);
        return CMD_ERROR;
    }
    
    // Create new node
    create_new_node();
    printf("Successfully created new node at index %d\n", CurrentnodeCount - 1);
    return CMD_SUCCESS;
}

int delete_node(unsigned int node_index) {
    printf("calling delete_node: %d\n", node_index);
    // Check if trying to delete garbage node
    if (node_index == GarbagenodeIndex) {
        return node_ERROR_GARBAGE;
    }
    
    // Check if node is in garbage cycle
    if (is_in_garbage_cycle(node_index)) {
        return node_ERROR_IN_GARBAGE_cycle;
    }
    
    uint node_position = CoreMap[node_index].core_position;
    printf("node_position: %d\n", node_position);
    initialize_node(&Core[node_position]);
    printf("node initialized\n");

    uint channel_offset = get_channel_offset(Core[GarbagenodeIndex], 0);
    uint axis_offset = get_axis_offset(Core[GarbagenodeIndex], 0, 0);
    uint first_garbage_node = *(uint*)(Core[GarbagenodeIndex] + channel_offset + axis_offset + 2);
    printf("First garbage node: %d\n", first_garbage_node);
    delete_link(GarbagenodeIndex, 0, first_garbage_node, 0, 0);
    create_link(GarbagenodeIndex, 0, node_index, 0, 0);
    create_link(node_index, 0, first_garbage_node, 0, 0);

    Core[node_position] = NULL;
    CoreMap[node_index].core_position = -1;
    CoreMap[node_index].is_loaded = 0;

    CoreSize--;
    return node_SUCCESS;
}
int handle_delete_node(char* args) {
    int node_index;
    
    // Parse arguments
    if (!args || sscanf(args, "%d", &node_index) != 1) {
        print_argument_error("delete-node", "<node_index>", false);
        return CMD_ERROR;
    }
    
    // Validate node index
    if (node_index < 0) {
        printf("Error: node index must be between 0 and 255\n");
        return CMD_ERROR;
    }

    // Can't delete garbage node
    if ((unsigned int)node_index == GarbagenodeIndex) {
        printf("Error: Cannot delete garbage node (index %d)\n", GarbagenodeIndex);
        return CMD_ERROR;
    }
    // printf("node_index: %d\n", node_index);
    // Delete the node
    int result = delete_node(node_index);
    // int result = node_SUCCESS;
    if (result == node_SUCCESS) {
        printf("Successfully deleted node %d\n", node_index);
    } else if (result == node_ERROR_GARBAGE) {
        printf("Error: Cannot delete garbage node (index %d)\n", GarbagenodeIndex);
    } else if (result == node_ERROR_IN_GARBAGE_cycle) {
        printf("Error: node %d is in garbage cycle\n", node_index);
    } else {
        printf("Failed to delete node %d\n", node_index);
    }
    
    return CMD_SUCCESS;
}
char* get_token_data(unsigned int node_index) {
    if (!validate_node(node_index)) {
        return NULL;
    }

    // Initialize stack
    Stack* stack = create_stack(MAX_STACK_SIZE);
    if (!stack) {
        printf("Error: Failed to create stack\n");
        return NULL;
    }

    // Initialize result buffer
    char* result = malloc(1024);  // Adjust size as needed
    if (!result) {
        destroy_stack(stack);
        return NULL;
    }
    int result_len = 0;

    // Push initial node
    if (!stack_push(stack, node_index, 0, 0)) {
        free(result);
        destroy_stack(stack);
        return NULL;
    }

    StackEntry current;
    while (stack_pop(stack, &current)) {
        // If node is a leaf node (0-255)
        if (current.node_index < 256) {
            result[result_len++] = (char)current.node_index;
            continue;
        }

        // Get children from axis 2
        uint node_position = get_node_position(current.node_index);
        uint channel_offset = get_channel_offset(Core[node_position], current.channel);
        uint axis_offset = get_axis_offset(Core[node_position], current.channel, TOKEN_DATA_AXIS);
        
        // Get link count
        ushort link_count = *(ushort*)(Core[node_position] + channel_offset + axis_offset);
        
        // Process links in reverse order (for correct data order)
        for (int i = link_count - 1; i >= 0; i--) {
            uint link_offset = channel_offset + axis_offset + 2 + (i * 6);
            uint child_node = *(uint*)(Core[node_position] + link_offset);
            ushort child_channel = *(ushort*)(Core[node_position] + link_offset + 4);

            // Push child to stack
            if (!stack_push(stack, child_node, child_channel, current.depth + 1)) {
                printf("Error: Stack overflow\n");
                free(result);
                destroy_stack(stack);
                return NULL;
            }
        }
    }

    destroy_stack(stack);
    result[result_len] = '\0';
    return result;
}

int create_token_node(unsigned int first_node, unsigned int second_node) {
    // Validate input vertices
    if (!validate_node(first_node) || !validate_node(second_node)) {
        return -1;
    }

    // Create new node
    create_new_node();
    uint new_node = CurrentnodeCount - 1;

    // Create axis 2 in new node for storing token data links
    if (create_axis(new_node, 0, TOKEN_DATA_AXIS) != AXIS_SUCCESS) {
        printf("Error: Failed to create token data axis\n");
        return -1;
    }

    // Link new node to first node's token search axis
    if (create_link(first_node, 0, new_node, 0, TOKEN_SEARCH_AXIS) != LINK_SUCCESS) {
        printf("Error: Failed to create search link\n");
        return -1;
    }

    // Link new node to both input vertices for data storage
    if (create_link(new_node, 0, first_node, 0, TOKEN_DATA_AXIS) != LINK_SUCCESS ||
        create_link(new_node, 0, second_node, 0, TOKEN_DATA_AXIS) != LINK_SUCCESS) {
        printf("Error: Failed to create data links\n");
        return -1;
    }

    return new_node;
}
int handle_get_token_data(char* args) {
    unsigned int node_index;
    
    // Parse node index
    if (sscanf(args, "%u", &node_index) != 1) {
        print_argument_error("get-token", "<node_index>", false);
        return CMD_ERROR;
    }
    
    // Get token data
    char* data = get_token_data(node_index);
    if (!data) {
        printf("Error: Failed to get token data from node %u\n", node_index);
        return CMD_ERROR;
    }
    
    // Print the data
    printf("Token data from node %u: %s\n", node_index, data);
    
    // Clean up
    free(data);
    return CMD_SUCCESS;
}

int handle_create_token(char* args) {
    unsigned int first_node, second_node;
    
    // Parse node indices
    if (sscanf(args, "%u %u", &first_node, &second_node) != 2) {
        print_argument_error("create-token", "<first_node> <second_node>", false);
        return CMD_ERROR;
    }
    
    // Create token node
    int new_node = create_token_node(first_node, second_node);
    if (new_node < 0) {
        printf("Error: Failed to create token node\n");
        return CMD_ERROR;
    }
    
    // Get and display the token data
    char* data = get_token_data(new_node);
    if (data) {
        printf("Successfully created token node %d with data: %s\n", new_node, data);
        free(data);
        return CMD_SUCCESS;
    }
    
    printf("Warning: Created token node %d but failed to read data\n", new_node);
    return CMD_SUCCESS;
}


TokenSearchResult* search_token(const char* data, size_t length) {
    if (!data || length == 0) {
        printf("Error: Invalid search data\n");
        return NULL;
    }
    printf("data: %s\n", data);
    TokenSearchResult* result = malloc(sizeof(TokenSearchResult));
    if (!result) {
        printf("Error: Failed to allocate result structure\n");
        return NULL;
    }

    // Start with first byte as node index
    uint current_node = (unsigned char)data[0];
    // printf("current_node: %d\n", current_node);
    size_t matched_pos = 1;
    
    // Get token data for current node
    char* token_data = get_token_data(current_node);
    printf("token_data: %s\n", token_data);
    if (!token_data) {
        printf("Error: Failed to get token data from node %u\n", current_node);
        free(result);
        return NULL;
    }

    // Search through token search axis (axis 0)
    while (matched_pos < length) {
        bool found_match = false;
        // Get links from current node
        uint node_position = get_node_position(current_node);
        // printf("node_position: %d\n", node_position);
        if (!Core[node_position]) break;

        uint channel_offset = get_channel_offset(Core[node_position], 0);  // Use channel 0
        if (!has_axis(Core[node_position], 0, TOKEN_SEARCH_AXIS)) {
            break;
        }
        uint axis_offset = get_axis_offset(Core[node_position], 0, TOKEN_SEARCH_AXIS);
        ushort link_count = *(ushort*)(Core[node_position] + channel_offset + axis_offset);
        int link_offset = channel_offset + axis_offset + 2;

        // Check each link's destination node
        for (int i = 0; i < link_count; i++) {
            uint next_node = *(uint*)(Core[node_position] + link_offset + (i * 6));
            char* next_token = get_token_data(next_node);
            if (!next_token) continue;

            // Check if next token matches remaining data
            size_t next_len = strlen(next_token);
            if (next_len <= length && 
                memcmp(data, next_token, next_len) == 0) {
                // Found matching next token
                free(token_data);
                token_data = next_token;
                current_node = next_node;
                matched_pos = next_len;
                found_match = true;
                break;
            }
            free(next_token);
        }

        if (!found_match) break;
    }

    // Store final result
    result->node_index = current_node;
    result->token_data = token_data;
    result->matched_length = matched_pos;
    // printf("result->matched_length: %d\n", result->matched_length);
    return result;
}

// Helper function to free search result
void free_search_result(TokenSearchResult* result) {
    if (result) {
        free(result->token_data);
        free(result);
    }
}

int handle_search_token(char* args) {
    // Set locale to support UTF-8
    setlocale(LC_ALL, "en_US.UTF-8");
    
    if (!args || !*args) {
        print_argument_error("search-token", "<text>", false);
        return CMD_ERROR;
    }

    // Search for matching token
    TokenSearchResult* result = search_token(args, strlen(args));
    if (!result) {
        printf("Error: Search failed\n");
        return CMD_ERROR;
    }

    // Print results
    printf("Search results for: %s\n", args);
    printf("Matched length: %d bytes\n", result->matched_length);
    printf("Final node: %u\n", result->node_index);
    
    // Print token data in multiple formats
    printf("Token data:\n");
    printf("Raw: %s\n", result->token_data);
    
    // Print hexadecimal format
    printf("HEX: ");
    for (size_t i = 0; result->token_data[i] != '\0'; i++) {
        printf("%02X ", (unsigned char)result->token_data[i]);
    }
    printf("\n");

    // Print UTF-8 format
    printf("UTF-8: ");
    fflush(stdout);  // Ensure proper output ordering
    fwrite(result->token_data, 1, strlen(result->token_data), stdout);
    printf("\n");

    free_search_result(result);
    return CMD_SUCCESS;
}
Vertex get_next_vertex(unsigned int node_index, unsigned short channel, unsigned short axis_number) {
    uint node_position = get_node_position(node_index);
    uint channel_offset = get_channel_offset(Core[node_position], channel);
    uint axis_offset = get_axis_offset(Core[node_position], channel, axis_number);
    uint link_offset = channel_offset + axis_offset + 2;
    Vertex vertex;
    vertex.node = *(uint*)(Core[node_position] + link_offset);
    vertex.channel = *(ushort*)(Core[node_position] + link_offset + 4);
    return vertex;
}
int integrate_token_data(unsigned int node_index) {
    // Check if node has enough channels for integration
    uint node_position = get_node_position(node_index);
    ushort channel_count = get_channel_count(Core[node_position]);
    if (channel_count <= 1) {
        return CMD_SUCCESS;
    }
    int new_channel_index = 1;
    uint new_node = 0;
    for (int i = 1; i < channel_count; i++) {
        bool new_node_created = false;
        if (get_link_count(node_index, i, STRING_AXIS) == 0) {
            continue;
        }
        Vertex next_vertex = get_next_vertex(node_index, i, STRING_AXIS);
        for (int j = i + 1; j < channel_count; j++) {
            if (get_link_count(node_index, j, STRING_AXIS) == 0) {
                continue;
            }
            Vertex next_vertex2 = get_next_vertex(node_index, j, STRING_AXIS);
            if (next_vertex.node == next_vertex2.node) {
                // Create combined token
                if (!new_node_created)
                {
                    new_node = create_token_node(node_index, next_vertex.node);
                    // if (create_multi_channels(new_node, 1) != CHANNEL_SUCCESS)
                    // {
                    //     printf("Error: Failed to create channels for node %u\n", new_node);
                    //     return ERROR;
                    // }
                    cycleInfo *existing_cycle = get_cycle_info(node_index, i, 2);

                    if (existing_cycle && existing_cycle->count == 2)
                    {
                        // printf("existing_cycle->count == 2\n");
                        clear_cycle(existing_cycle);
                        create_channel(new_node);
                        create_loop(new_node, get_channel_count(Core[new_node]) - 1, 2);
                        new_channel_index++;
                    }
                    else
                    {
                        create_channel(new_node);
                        Vertex new_vertex;
                        new_vertex.node = new_node;
                        new_vertex.channel = get_channel_count(Core[new_node]) - 1;
                        Vertex old_vertex;
                        old_vertex.node = existing_cycle->vertices[0];
                        old_vertex.channel = existing_cycle->channels[0];
                        // if (existing_cycle->count > 2)
                        // if (get_channel_count(Core[new_node]) < new_channel_index)
                        // {
                        //     create_channel(new_node);
                        // }
                            replace_new_token(new_vertex, old_vertex, 2);
                        // if (existing_cycle)
                        //     free_cycle_info(existing_cycle);

                        new_channel_index++;
                    }
                    new_node_created = true;
                }
                cycleInfo *existing_cycle = get_cycle_info(node_index, j, 2);

                if (existing_cycle && existing_cycle->count == 2)
                {
                    // printf("existing_cycle->count == 2\n");
                    clear_cycle(existing_cycle);
                    create_channel(new_node);
                    create_loop(new_node, get_channel_count(Core[new_node]) - 1, 2);
                    new_channel_index++;
                }
                else
                {
                    create_channel(new_node);
                    Vertex new_vertex;
                    new_vertex.node = new_node;
                    new_vertex.channel = get_channel_count(Core[new_node]) - 1;
                    Vertex old_vertex;
                    old_vertex.node = existing_cycle->vertices[0];
                    old_vertex.channel = existing_cycle->channels[0];
                    replace_new_token(new_vertex, old_vertex, 2);
                    new_channel_index++;
                }
            }
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
        return CMD_ERROR;
    }
    
    // Validate input
    if (!validate_node(node_index)) {
        return CMD_ERROR;
    }
    
    // Integrate tokens
    int result = integrate_token_data(node_index);
    if (result == SUCCESS) {
        printf("Successfully integrated tokens in node %d\n", node_index);
        return CMD_SUCCESS;
    } else {
        printf("Failed to integrate tokens in node %d\n", node_index);
        return CMD_ERROR;
    }
}
int load_current_vertex() {
    if (get_link(pointer_current_vertex, 0, PROPERTY_AXIS, 0, &CurrentVertex.node, &CurrentVertex.channel) == LINK_SUCCESS) {
        return SUCCESS;
    }
    return ERROR;
}
int update_current_vertex() {
    delete_first_link(pointer_current_vertex, 0, STRING_AXIS);
    create_link(pointer_current_vertex, 0, CurrentVertex.node, CurrentVertex.channel, PROPERTY_AXIS);
    return SUCCESS;
}
int update_current_vertex_to_root() {
    delete_first_link(pointer_current_vertex, 0, STRING_AXIS);
    create_link(pointer_current_vertex, 0, RootVertex.node, RootVertex.channel, PROPERTY_AXIS);
    return SUCCESS;
}