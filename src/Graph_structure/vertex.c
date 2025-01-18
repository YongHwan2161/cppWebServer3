#include "vertex.h"
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

// Initial vertex values
static uchar initValues[16] = {
    4,  0,     // allocated size power (2^4 = 16 bytes)
    14, 0, 0, 0,  // actual used size (14 bytes initially)
    1,  0,     // number of channels (1)
    12, 0, 0, 0,   // offset for channel 0 (starts at byte 12)
    0,  0,     // number of axes (0)
    0,  0      // remaining bytes initialized to 0
};

/**
 * Validates if a vertex index is valid and the vertex exists in Core
 * 
 * @param vertex_index Index of the vertex to validate
 * @return true if valid, false if invalid
 */
bool validate_vertex(unsigned int vertex_index) {
    // Check if vertex exists in Core
    uint vertex_position = get_vertex_position(vertex_index);
    if (!Core[vertex_position]) {
        printf("Error: Invalid vertex index %u\n", vertex_index);
        return false;
    }

    return true;
}

/**
 * Saves vertex data to data.bin file and updates map.bin
 * 
 * @param vertex_index Index of the vertex to save
 * @return true if successful, false if failed
 */
bool save_vertex_to_file(unsigned int vertex_index) {
    if (!validate_vertex(vertex_index)) {
        return false;
    }
    // printf("start save_vertex_to_file\n");
    uint vertex_position = CoreMap[vertex_index].core_position;
    uchar* vertex = Core[vertex_position];
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
    printf("file offset: 0x%lx\n", CoreMap[vertex_index].file_offset);
    // Write vertex data
    if (fseek(data_file, CoreMap[vertex_index].file_offset, SEEK_SET) != 0) {
        printf("Error: Failed to seek in data.bin\n");
        fclose(data_file);
        return false;
    }

    size_t vertex_size = 1 << (*(ushort*)vertex);
    if (fwrite(vertex, 1, vertex_size, data_file) != vertex_size) {
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
        // Write initial vertex count
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
    if (fseek(map_file, sizeof(uint) + (vertex_index * sizeof(long)), SEEK_SET) != 0) {
        printf("Error: Failed to seek in map.bin\n");
        fclose(map_file);
        return false;
    }

    if (fwrite(&CoreMap[vertex_index].file_offset, sizeof(long), 1, map_file) != 1) {
        printf("Error: Failed to write to map.bin\n");
        fclose(map_file);
        return false;
    }
    
    fclose(map_file);
    return true;
}

bool save_current_vertex_count() {
    FILE* map_file = fopen(MAP_FILE, "r+b");
    if (!map_file) {
        printf("Error: Failed to open map.bin in save_current_vertex_count\n");
        map_file = fopen(MAP_FILE, "wb");
        if (!map_file) {
            printf("Error: Failed to create map.bin in save_current_vertex_count\n");
            return false;
        }
    }
    fwrite(&CurrentvertexCount, sizeof(uint), 1, map_file);
    fclose(map_file);
    return true;
}
void initialize_vertex(uchar** vertex) {
    for (int i = 0; i < 16; ++i) {
        (*vertex)[i] = initValues[i];
    }
}
void create_new_vertex() {
    uchar* newvertex = (uchar*)malloc(16 * sizeof(uchar));  // Always allocate 16 bytes initially
    // printf("Creating new vertex at index %d\n", CurrentvertexCount);
    initialize_vertex(&newvertex);
    CurrentvertexCount++;
    save_current_vertex_count();
    Core[CurrentvertexCount - 1] = newvertex;
    CoreSize++;
    CoreMap[CurrentvertexCount - 1].core_position = CurrentvertexCount - 1;
    CoreMap[CurrentvertexCount - 1].is_loaded = 1;
    if (CurrentvertexCount == 1) {
        CoreMap[CurrentvertexCount - 1].file_offset = 0;
    } else {
        uint last_vertex_size = 1 << (*(ushort*)Core[CurrentvertexCount - 2]);
        // printf("Last vertex size: %d\n", last_vertex_size);
        uint file_offset = CoreMap[CurrentvertexCount - 2].file_offset + last_vertex_size;
        CoreMap[CurrentvertexCount - 1].file_offset = file_offset;
    }
    save_vertex_to_file(CurrentvertexCount - 1);
    printf("vertex created at index %d\n", CurrentvertexCount - 1);
}

int handle_create_vertex(char* args) {
    // No arguments needed, but check if any were provided
    if (args && *args != '\0') {
        print_argument_error("create-vertex", "", false);
        return CMD_ERROR;
    }
    
    // Check if we've reached the maximum number of vertices
    if (CurrentvertexCount >= MaxCoreSize) {
        printf("Error: Maximum number of vertices (%d) reached\n", MaxCoreSize);
        return CMD_ERROR;
    }
    
    // Create new vertex
    create_new_vertex();
    printf("Successfully created new vertex at index %d\n", CurrentvertexCount - 1);
    return CMD_SUCCESS;
}

int delete_vertex(unsigned int vertex_index) {
    printf("calling delete_vertex: %d\n", vertex_index);
    // Check if trying to delete garbage vertex
    if (vertex_index == GarbagevertexIndex) {
        return VERTEX_ERROR_GARBAGE;
    }
    
    // Check if vertex is in garbage cycle
    if (is_in_garbage_cycle(vertex_index)) {
        return VERTEX_ERROR_IN_GARBAGE_cycle;
    }
    
    uint vertex_position = CoreMap[vertex_index].core_position;
    printf("vertex_position: %d\n", vertex_position);
    initialize_vertex(&Core[vertex_position]);
    printf("vertex initialized\n");

    uint channel_offset = get_channel_offset(Core[GarbagevertexIndex], 0);
    uint axis_offset = get_axis_offset(Core[GarbagevertexIndex], 0, 0);
    uint first_garbage_vertex = *(uint*)(Core[GarbagevertexIndex] + channel_offset + axis_offset + 2);
    printf("First garbage vertex: %d\n", first_garbage_vertex);
    delete_link(GarbagevertexIndex, 0, first_garbage_vertex, 0, 0);
    create_link(GarbagevertexIndex, 0, vertex_index, 0, 0);
    create_link(vertex_index, 0, first_garbage_vertex, 0, 0);

    Core[vertex_position] = NULL;
    CoreMap[vertex_index].core_position = -1;
    CoreMap[vertex_index].is_loaded = 0;

    CoreSize--;
    return VERTEX_SUCCESS;
}
int handle_delete_vertex(char* args) {
    int vertex_index;
    
    // Parse arguments
    if (!args || sscanf(args, "%d", &vertex_index) != 1) {
        print_argument_error("delete-vertex", "<vertex_index>", false);
        return CMD_ERROR;
    }
    
    // Validate vertex index
    if (vertex_index < 0) {
        printf("Error: vertex index must be between 0 and 255\n");
        return CMD_ERROR;
    }

    // Can't delete garbage vertex
    if ((unsigned int)vertex_index == GarbagevertexIndex) {
        printf("Error: Cannot delete garbage vertex (index %d)\n", GarbagevertexIndex);
        return CMD_ERROR;
    }
    // printf("vertex_index: %d\n", vertex_index);
    // Delete the vertex
    int result = delete_vertex(vertex_index);
    // int result = VERTEX_SUCCESS;
    if (result == VERTEX_SUCCESS) {
        printf("Successfully deleted vertex %d\n", vertex_index);
    } else if (result == VERTEX_ERROR_GARBAGE) {
        printf("Error: Cannot delete garbage vertex (index %d)\n", GarbagevertexIndex);
    } else if (result == VERTEX_ERROR_IN_GARBAGE_cycle) {
        printf("Error: Vertex %d is in garbage cycle\n", vertex_index);
    } else {
        printf("Failed to delete vertex %d\n", vertex_index);
    }
    
    return CMD_SUCCESS;
}
char* get_token_data(unsigned int vertex_index) {
    if (!validate_vertex(vertex_index)) {
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

    // Push initial vertex
    if (!stack_push(stack, vertex_index, 0, 0)) {
        free(result);
        destroy_stack(stack);
        return NULL;
    }

    StackEntry current;
    while (stack_pop(stack, &current)) {
        // If vertex is a leaf node (0-255)
        if (current.vertex_index < 256) {
            result[result_len++] = (char)current.vertex_index;
            continue;
        }

        // Get children from axis 2
        uint vertex_position = get_vertex_position(current.vertex_index);
        uint channel_offset = get_channel_offset(Core[vertex_position], current.channel);
        uint axis_offset = get_axis_offset(Core[vertex_position], current.channel, TOKEN_DATA_AXIS);
        
        // Get link count
        ushort link_count = *(ushort*)(Core[vertex_position] + channel_offset + axis_offset);
        
        // Process links in reverse order (for correct data order)
        for (int i = link_count - 1; i >= 0; i--) {
            uint link_offset = channel_offset + axis_offset + 2 + (i * 6);
            uint child_vertex = *(uint*)(Core[vertex_position] + link_offset);
            ushort child_channel = *(ushort*)(Core[vertex_position] + link_offset + 4);

            // Push child to stack
            if (!stack_push(stack, child_vertex, child_channel, current.depth + 1)) {
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

int create_token_vertex(unsigned int first_vertex, unsigned int second_vertex) {
    // Validate input vertices
    if (!validate_vertex(first_vertex) || !validate_vertex(second_vertex)) {
        return -1;
    }

    // Create new vertex
    create_new_vertex();
    uint new_vertex = CurrentvertexCount - 1;

    // Create axis 2 in new vertex for storing token data links
    if (create_axis(new_vertex, 0, TOKEN_DATA_AXIS) != AXIS_SUCCESS) {
        printf("Error: Failed to create token data axis\n");
        return -1;
    }

    // Link new vertex to first vertex's token search axis
    if (create_link(first_vertex, 0, new_vertex, 0, TOKEN_SEARCH_AXIS) != LINK_SUCCESS) {
        printf("Error: Failed to create search link\n");
        return -1;
    }

    // Link new vertex to both input vertices for data storage
    if (create_link(new_vertex, 0, first_vertex, 0, TOKEN_DATA_AXIS) != LINK_SUCCESS ||
        create_link(new_vertex, 0, second_vertex, 0, TOKEN_DATA_AXIS) != LINK_SUCCESS) {
        printf("Error: Failed to create data links\n");
        return -1;
    }

    return new_vertex;
}
int handle_get_token_data(char* args) {
    unsigned int vertex_index;
    
    // Parse vertex index
    if (sscanf(args, "%u", &vertex_index) != 1) {
        print_argument_error("get-token", "<vertex_index>", false);
        return CMD_ERROR;
    }
    
    // Get token data
    char* data = get_token_data(vertex_index);
    if (!data) {
        printf("Error: Failed to get token data from vertex %u\n", vertex_index);
        return CMD_ERROR;
    }
    
    // Print the data
    printf("Token data from vertex %u: %s\n", vertex_index, data);
    
    // Clean up
    free(data);
    return CMD_SUCCESS;
}

int handle_create_token(char* args) {
    unsigned int first_vertex, second_vertex;
    
    // Parse vertex indices
    if (sscanf(args, "%u %u", &first_vertex, &second_vertex) != 2) {
        print_argument_error("create-token", "<first_vertex> <second_vertex>", false);
        return CMD_ERROR;
    }
    
    // Create token vertex
    int new_vertex = create_token_vertex(first_vertex, second_vertex);
    if (new_vertex < 0) {
        printf("Error: Failed to create token vertex\n");
        return CMD_ERROR;
    }
    
    // Get and display the token data
    char* data = get_token_data(new_vertex);
    if (data) {
        printf("Successfully created token vertex %d with data: %s\n", new_vertex, data);
        free(data);
        return CMD_SUCCESS;
    }
    
    printf("Warning: Created token vertex %d but failed to read data\n", new_vertex);
    return CMD_SUCCESS;
}

// Structure to hold search result
typedef struct {
    uint vertex_index;    // Found vertex index
    char* token_data;     // Token data at vertex
    int matched_length;   // Length of matched data
} TokenSearchResult;

TokenSearchResult* search_token(const char* data, size_t length) {
    if (!data || length == 0) {
        printf("Error: Invalid search data\n");
        return NULL;
    }

    TokenSearchResult* result = malloc(sizeof(TokenSearchResult));
    if (!result) {
        printf("Error: Failed to allocate result structure\n");
        return NULL;
    }

    // Start with first byte as vertex index
    uint current_vertex = (unsigned char)data[0];
    size_t matched_pos = 0;
    
    // Get token data for current vertex
    char* token_data = get_token_data(current_vertex);
    if (!token_data) {
        printf("Error: Failed to get token data from vertex %u\n", current_vertex);
        free(result);
        return NULL;
    }

    // Search through token search axis (axis 0)
    while (matched_pos < length) {
        bool found_match = false;
        // Get links from current vertex
        uint vertex_position = get_vertex_position(current_vertex);
        printf("vertex_position: %d\n", vertex_position);
        if (!Core[vertex_position]) break;

        uint channel_offset = get_channel_offset(Core[vertex_position], 0);  // Use channel 0
        if (!has_axis(Core[vertex_position], 0, TOKEN_SEARCH_AXIS)) {
            break;
        }
        uint axis_offset = get_axis_offset(Core[vertex_position], 0, TOKEN_SEARCH_AXIS);
        ushort link_count = *(ushort*)(Core[vertex_position] + channel_offset + axis_offset);
        printf("link_count: %d\n", link_count);
        int link_offset = channel_offset + axis_offset + 2;

        // Check each link's destination vertex
        for (int i = 0; i < link_count; i++) {
            uint next_vertex = *(uint*)(Core[vertex_position] + link_offset + (i * 6));
            printf("next_vertex: %d\n", next_vertex);
            char* next_token = get_token_data(next_vertex);
            printf("next_token: %s\n", next_token);
            if (!next_token) continue;

            // Check if next token matches remaining data
            size_t next_len = strlen(next_token);
            printf("next_len: %ld\n", next_len);
            if (next_len <= length && 
                memcmp(data, next_token, next_len) == 0) {
                // Found matching next token
                free(token_data);
                token_data = next_token;
                current_vertex = next_vertex;
                matched_pos = next_len;
                found_match = true;
                break;
            }
            free(next_token);
        }

        if (!found_match) break;
    }

    // Store final result
    result->vertex_index = current_vertex;
    result->token_data = token_data;
    result->matched_length = matched_pos;
    
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
    printf("Final vertex: %u\n", result->vertex_index);
    
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