#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include "node.h"
#include "../../../CGDB.h"
#include "../../map.h"
#include "../../data_structures/stack.h"
#include "../../cli/command_handler.h"
#include "../../database.h"
#include "../../memory.h"
#include "../link.h"  // For Core array access
#include "../cycle.h"
#include "../vertex.h"
#include "../channel.h"
#include "../axis.h"

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
    long node_position = get_node_position(node_index);
    if (node_position == -1) {
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
    long node_position = get_node_position(node_index);
    if (node_position == -1) return false;
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

    ushort node_size_power = *(ushort*)node;
    size_t node_size = (size_t)1 << node_size_power;
    
    if (fwrite(node, 1, node_size, data_file) != node_size) {
        printf("Error: Failed to write to data.bin, save_node_to_file(%d)\n", node_index);
        fclose(data_file);
        return false;
    }
    fclose(data_file);

    // Try to open map file, create if doesn't exist
    FILE* map_file = fopen(MAP_FILE, "r+b");
    if (!map_file) {
        map_file = fopen(MAP_FILE, "wb");
        if (!map_file) {
            printf("Error: Failed to create map.bin, save_node_to_file(%d)\n", node_index);
            return false;
        }
        fclose(map_file);
        map_file = fopen(MAP_FILE, "r+b");
    }

    // Update map entry
    if (fseek(map_file, sizeof(uint) + (node_index * sizeof(long)), SEEK_SET) != 0) {
        printf("Error: Failed to seek in map.bin, save_node_to_file(%d)\n", node_index);
        fclose(map_file);
        return false;
    }

    if (fwrite(&CoreMap[node_index].file_offset, sizeof(long), 1, map_file) != 1) {
        printf("Error: Failed to write to map.bin, save_node_to_file(%d)\n", node_index);
        fclose(map_file);
        return false;
    }
    
    fclose(map_file);
    return true;
}
bool save_all_nodes() {
    // Try to open data file, create if doesn't exist
    // if (!data_file) {
    //     data_file = fopen(DATA_FILE, "wb");
    //     if (!data_file) {
    //         printf("Error: Failed to create data.bin\n");
    //         return false;
    //     }
    //     fclose(data_file);
    //     data_file = fopen(DATA_FILE, "r+b");
    // }

    // Save each node
    for (unsigned int node_index = 0; node_index < CurrentnodeCount; node_index++) {
        
    FILE *data_file = fopen(DATA_FILE, "r+b");
        long node_position = get_node_position(node_index);
        if (node_position == -1) {
            fclose(data_file);
            return false;
        }

        uchar *node = Core[node_position];
        
        // Write node data
        if (fseek(data_file, CoreMap[node_index].file_offset, SEEK_SET) != 0) {
            printf("Error: Failed to seek in data.bin, save_node_to_file(%d)\n", node_index);
            fclose(data_file);
            return false;
        }
        ushort node_size_power = *(ushort*)node;
        size_t node_size = (size_t)1 << node_size_power;
        if (fwrite(node, 1, node_size, data_file) != node_size) {
            printf("Error: Failed to write to data.bin, save_node_to_file(%d)\n", node_index);
            fclose(data_file);
            return false;
        }
    fclose(data_file);
    }

    // Try to open map file, create if doesn't exist
    // if (!map_file) {
    //     map_file = fopen(MAP_FILE, "wb");
    //     if (!map_file) {
    //         fclose(map_file);
    //         printf("Error: Failed to create map.bin\n");
    //         return false;
    //     }
    //     fclose(map_file);
    //     map_file = fopen(MAP_FILE, "r+b");
    // }
    for (unsigned int node_index = 0; node_index < CurrentnodeCount; node_index++)
    {
        FILE *map_file = fopen(MAP_FILE, "r+b");
        // Update map entry
        if (fseek(map_file, sizeof(uint) + (node_index * sizeof(long)), SEEK_SET) != 0)
        {
            printf("Error: Failed to seek in map.bin, save_all_nodes(%d)\n", node_index);
            fclose(map_file);
            return false;
        }

        if (fwrite(&CoreMap[node_index].file_offset, sizeof(long), 1, map_file) != 1)
        {
            printf("Error: Failed to write to map.bin, save_all_nodes(%d)\n", node_index);
            fclose(map_file);
            return false;
        }
        fclose(map_file);
    }

    return true;
}
bool save_inconsistent_nodes() {
    for (unsigned int i = 0; i < CurrentnodeCount; i++) {
        if (!check_node_consistency(i)) {
            printf("Node %d is inconsistent\n", i);
            save_node_to_file(i);
        }
    }
    return true;
}
int handle_save_inconsistent_nodes() {
    if (save_inconsistent_nodes()) {
        printf("Successfully saved inconsistent nodes\n");
        return CMD_SUCCESS;
    }
    printf("Error: Failed to save inconsistent nodes\n");
    return CMD_ERROR;
}
int handle_save_node(char* args) {
    if (!args) {
        print_argument_error("save-node", "", false);
        return ERROR;
    }
    int node_index = atoi(args);
    return save_node_to_file(node_index) ? SUCCESS : ERROR;
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
int create_new_node() {
    uchar* newnode = (uchar*)malloc(16 * sizeof(uchar));
    if (!newnode) {
        printf("Error: Failed to allocate memory for new node\n");
        return ERROR;
    }
    
    initialize_node(&newnode);
    CurrentnodeCount++;
    save_current_node_count();
    
    Core[CurrentnodeCount - 1] = newnode;
    CoreSize++;
    CoreMap[CurrentnodeCount - 1].core_position = CurrentnodeCount - 1;
    CoreMap[CurrentnodeCount - 1].is_loaded = 1;
    
    CoreMap[CurrentnodeCount - 1].file_offset = max_offset;
    max_offset += 16;
    
    if (!save_node_to_file(CurrentnodeCount - 1)) {
        printf("Error: Failed to save node\n");
        free(newnode);
        return ERROR;
    }
    
    printf("Node created at index %d\n", CurrentnodeCount - 1);
    return SUCCESS;
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

int delete_node(unsigned int node_index, bool save) {
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
    delete_link(GarbagenodeIndex, 0, first_garbage_node, 0, 0, save);
    create_link(GarbagenodeIndex, 0, node_index, 0, 0, save);
    create_link(node_index, 0, first_garbage_node, 0, 0, save);

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
    int result = delete_node(node_index, true);
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
        long node_position = get_node_position(current.node_index);
        if (node_position == -1) continue;
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

/**
 * Creates or updates token search tree for a new token node
 * 
 * @param new_node Index of the new token node
 * @param new_token_data Token data to insert into search tree
 * @param first_node Starting node for search tree traversal
 * @return SUCCESS if tree was updated successfully, ERROR otherwise
 */
int create_token_search_tree(uint new_node, const char* new_token_data, uint first_node, bool save) {
    if (!new_token_data || !validate_node(new_node) || !validate_node(first_node)) {
        return ERROR;
    }

    // Start from first_node and traverse token search tree
    uint current_node = first_node;
    bool link_created = false;

    while (!link_created) {
        // Get all linked nodes through token search axis (0)
        long node_position = get_node_position(current_node);
        if (node_position == -1) return ERROR;

        uint channel_offset = get_channel_offset(Core[node_position], 0);
        if (!has_axis(Core[node_position], 0, TOKEN_SEARCH_AXIS)) {
            // Create link from current node to new node if no further paths
            create_link(current_node, 0, new_node, 0, TOKEN_SEARCH_AXIS, save);
            return SUCCESS;
        }

        uint axis_offset = get_axis_offset(Core[node_position], 0, TOKEN_SEARCH_AXIS);
        ushort link_count = *(ushort*)(Core[node_position] + channel_offset + axis_offset);
        int link_offset = channel_offset + axis_offset + 2;

        bool found_path = false;
        
        // Check each linked node's token data
        for (int i = 0; i < link_count; i++) {
            uint dest_node = *(uint*)(Core[node_position] + link_offset + (i * 6));
            char* dest_token = get_token_data(dest_node);
            if (!dest_token) continue;

            // If destination token is prefix of new token, continue down that path
            if (strncmp(new_token_data, dest_token, strlen(dest_token)) == 0) {
                current_node = dest_node;
                found_path = true;
                free(dest_token);
                break;
            }
            free(dest_token);
        }

        // If no matching path found, create link from current node to new node
        if (!found_path) {
            create_link(current_node, 0, new_node, 0, TOKEN_SEARCH_AXIS, save);
            link_created = true;
        }
    }

    return SUCCESS;
}

int create_token_node(unsigned int first_node, unsigned int second_node, bool sync) {
    // Validate input vertices
    if (!validate_node(first_node) || !validate_node(second_node)) {
        return -1;
    }

    // Create new node
    create_new_node();
    uint new_node = CurrentnodeCount - 1;

    // Create axis 2 in new node for storing token data links
    if (create_axis(new_node, 0, TOKEN_DATA_AXIS, sync) != AXIS_SUCCESS) {
        printf("Error: Failed to create token data axis\n");
        return -1;
    }

    // Link new node to first node's token search axis
    // if (create_link(first_node, 0, new_node, 0, TOKEN_SEARCH_AXIS, sync) != LINK_SUCCESS) {
    //     printf("Error: Failed to create search link\n");
    //     return -1;
    // }

    // Link new node to both input vertices for data storage
    if (create_link(new_node, 0, first_node, 0, TOKEN_DATA_AXIS, sync) != LINK_SUCCESS ||
        create_link(new_node, 0, second_node, 0, TOKEN_DATA_AXIS, sync) != LINK_SUCCESS) {
        printf("Error: Failed to create data links\n");
        return -1;
    }

    char* first_token_data = get_token_data(first_node);
    char* second_token_data = get_token_data(second_node);
    char* new_token_data = strcat(first_token_data, second_token_data);
    create_token_search_tree(new_node, new_token_data, first_node, sync);

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
    int new_node = create_token_node(first_node, second_node, true);
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
    // printf("data: %s\n", data);
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
    // printf("token_data: %s\n", token_data);
    if (!token_data) {
        printf("Error: Failed to get token data from node %u\n", current_node);
        free(result);
        return NULL;
    }

    // Search through token search axis (axis 0)
    while (matched_pos < length) {
        bool found_match = false;
        // Get links from current node
        long node_position = get_node_position(current_node);
        // printf("node_position: %d\n", node_position);
        if (node_position == -1) break;

        uint channel_offset = get_channel_offset(Core[node_position], 0);  // Use channel 0
        if (channel_offset == 0) {
            break;
        }
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
