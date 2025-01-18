#include "cycle.h"
#include "../memory.h"
#include "../map.h"
#include "axis.h"
#include "channel.h"
#include "link.h"
#include "vertex.h"
#include "../cli/command_handler.h"
#include <stdlib.h>
#include <locale.h>

#define MAX_cycle_vertices 1000

typedef struct {
    uint vertex;
    ushort channel;
    ushort axis;
} Pathvertex;

bool has_cycle(unsigned int vertex_index, ushort channel_index, ushort axis_number) {
    Pathvertex* visited = (Pathvertex*)malloc(MAX_cycle_vertices * sizeof(Pathvertex));
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
    Pathvertex* visited = (Pathvertex*)malloc(MAX_cycle_vertices * sizeof(Pathvertex));
    cycleInfo* info = (cycleInfo*)malloc(sizeof(cycleInfo));
    info->vertices = (uint*)malloc(MAX_cycle_vertices * sizeof(uint));
    info->channels = (ushort*)malloc(MAX_cycle_vertices * sizeof(ushort));
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
    if (!token_vertices) {
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

char* get_sentence_data(uint vertex_index, ushort channel_index) {
    // Get cycle info
    cycleInfo* info = get_cycle_info(vertex_index, channel_index, SENTENCE_AXIS);
    if (!info || info->count < 2) {
        printf("Error: No valid sentence cycle found\n");
        free_cycle_info(info);
        return NULL;
    }

    // Allocate buffer for sentence data
    char* sentence = (char*)malloc(1024);  // Add explicit cast to char*
    if (!sentence) {
        free_cycle_info(info);
        return NULL;
    }
    int sentence_len = 0;

    // Get token data for each vertex in cycle
    for (int i = 0; i < info->count; i++) {
        char* token_data = get_token_data(info->vertices[i]);
        if (!token_data) {
            printf("Error: Failed to get token data from vertex %u\n", info->vertices[i]);
            free(sentence);
            free_cycle_info(info);
            return NULL;
        }

        // Add token data to sentence
        int token_len = strlen(token_data);
        if (sentence_len + token_len >= 1023) {
            printf("Error: Sentence too long\n");
            free(token_data);
            free(sentence);
            free_cycle_info(info);
            return NULL;
        }

        // Copy token data without adding spaces
        memcpy(sentence + sentence_len, token_data, token_len);
        sentence_len += token_len;

        free(token_data);
    }

    sentence[sentence_len] = '\0';
    free_cycle_info(info);
    return sentence;
} 
#define MAX_SENTENCE_TOKENS 100

int handle_create_sentence(char* args) {
    if (!args || !*args) {
        print_argument_error("create-sentence", "<text>", false);
        return ERROR;
    }

    uint tokens[MAX_SENTENCE_TOKENS];
    ushort channels[MAX_SENTENCE_TOKENS];
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
        printf("result->matched_length == remaining_len\n");
        int channel_index = recycle_or_create_channel(result_first->vertex_index);
        if (channel_index == CHANNEL_ERROR)
        {
            printf("Error: Failed to create channel for vertex %u\n", result_first->vertex_index);
            free_search_result(result_first);
            return ERROR;
        }

        if (create_loop(result_first->vertex_index, channel_index, 2) != LINK_SUCCESS)
        {
            printf("Error: Failed to create loop for vertex %u\n", result_first->vertex_index);
            free_search_result(result_first);
            return ERROR;
        }
        free_search_result(result_first);
        return SUCCESS;
    }

    // Tokenize input using search_token
    while (remaining_len > 0 && count < MAX_SENTENCE_TOKENS) {
        TokenSearchResult *result = search_token(current_pos, remaining_len);
        if (!result) break;

        tokens[count] = result->vertex_index;
        channels[count] = recycle_or_create_channel(result->vertex_index);
        if (channels[count] == CHANNEL_ERROR) {
            printf("Error: Failed to create channel for vertex %u\n", result->vertex_index);
            free_search_result(result);
            return ERROR;
        }

        // Check for possible token combinations
        if (count > 0) {
            uint prev_vertex = tokens[count-1];
            uint vertex_position = get_vertex_position(prev_vertex);
            if (!Core[vertex_position]) {
                continue;
            }

            ushort channel_count = get_channel_count(Core[vertex_position]);
            if (channel_count == 1) {
                create_channel(prev_vertex); 
                create_channel(tokens[count]);
                channels[count] = 1;
                count++;
                break;
            }
            // Check each channel for matching next token
            for (ushort ch = 1; ch < channel_count; ch++) {
                cycleInfo* cycle = get_cycle_info(prev_vertex, ch, 2);
                if (!cycle) continue;

                // Compare next vertex's token data with current token
                // for (int i = 0; i < cycle->count; i++) {
                    char* next_token = get_token_data(tokens[count]);
                    printf("next_token: %s\n", next_token);
                    if (!next_token) continue;

                    if (strcmp(next_token, result->token_data) == 0) {
                        // Create combined token
                        int new_vertex = create_token_vertex(prev_vertex, result->vertex_index);
                        create_channel(new_vertex);

                        if (new_vertex >= 0) {
                            cycleInfo* existing_cycle = get_cycle_info(prev_vertex, ch, 2);
                            
                            if (existing_cycle && existing_cycle->count == 2) {
                                printf("existing_cycle->count == 2\n");
                                // Special case: 2-token cycle
                                // Clear channels of both tokens in existing cycle
                                clear_channel(existing_cycle->vertices[0], existing_cycle->channels[0]);
                                clear_channel(existing_cycle->vertices[1], existing_cycle->channels[1]);
                                
                                // Create self-loop for combined token
                                create_link(new_vertex, 1, new_vertex, 1, 2);
                            } else {
                                // Normal case: Remove old tokens and insert new one
                                delete_path_from_cycle(tokens[count-1], channels[count-1], 2, 2);                            
                            if (existing_cycle) free_cycle_info(existing_cycle);
                            
                            uint new_path[1] = {(uint)new_vertex};
                            ushort new_channels[1] = {0};
                            insert_path_into_cycle(tokens[count-1], channels[count-1],
                                                 new_path, new_channels, 1, 2);
                            
                            }

                            // Update tokens array
                            tokens[count-1] = new_vertex;
                            count--;
                        }
                        free(next_token);
                        break;
                    }
                    free(next_token);
                // }
                free_cycle_info(cycle);
            }
        }

        current_pos += result->matched_length;
        remaining_len -= result->matched_length;

        count++;
        free_search_result(result);
    }

    if (count < 2) {
        printf("Error: At least 2 tokens required for a sentence\n");
        return ERROR;
    }

    // Create sentence cycle using the tokens
    if (create_sentence_cycle(tokens, count) == LINK_SUCCESS) {
        printf("Successfully created sentence cycle with %d tokens\n", count);
        return SUCCESS;
    }

    printf("Error: Failed to create sentence cycle\n");
    return ERROR;
}
int handle_get_sentence(char* args) {
    // Set locale to support UTF-8
    setlocale(LC_ALL, "en_US.UTF-8");
    
    uint vertex_index;
    ushort channel_index;
    
    // Parse arguments
    if (sscanf(args, "%u %hu", &vertex_index, &channel_index) != 2) {
        print_argument_error("get-sentence", "<vertex_index> <channel_index>", false);
        return CMD_ERROR;
    }
    
    // Get sentence data
    char* data = get_sentence_data(vertex_index, channel_index);
    if (!data) {
        printf("Error: Failed to get sentence data\n");
        return CMD_ERROR;
    }
    
    // Print the data in multiple formats
    printf("Sentence data starting from vertex %u, channel %u:\n", vertex_index, channel_index);
    
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
        print_argument_error("create-cycle", "<vertex1> <ch1> <vertex2> <ch2> ... <axis>", false);
        return CMD_ERROR;
    }
    
    // Parse axis number
    if (sscanf(last_space + 1, "%d", &axis_number) != 1) {
        printf("Error: Invalid axis number\n");
        return CMD_ERROR;
    }
    
    // Null terminate the vertex/channel list for parsing
    *last_space = '\0';
    
    // Parse vertex and channel pairs
    char* token = strtok(args, " ");
    while (token && count < MAX_CMD_CYCLE_VERTICES) {
        // Parse vertex
        if (sscanf(token, "%u", &vertices[count]) != 1) {
            printf("Error: Invalid vertex index\n");
            return CMD_ERROR;
        }
        
        // Get next token for channel
        token = strtok(NULL, " ");
        if (!token) {
            printf("Error: Missing channel for vertex %u\n", vertices[count]);
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

    // Check if any vertex in the proposed cycle already has a cycle
    for (int i = 0; i < count; i++) {
        if (has_cycle(vertices[i], channels[i], axis_number)) {
            printf("Error: vertex %u channel %u already belongs to a cycle on axis %d\n", 
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

bool validate_cycle(int vertex_index, int channel_index, int axis_number) {
    cycleInfo* info = get_cycle_info(vertex_index, channel_index, axis_number);
    bool has_cycle = info->count > 0;
    
    if (has_cycle) {
        printf("Found cycle with %d vertices:\n", info->count);
        for (int i = 0; i < info->count; i++) {
            printf("vertex %u, Channel %u\n", info->vertices[i], info->channels[i]);
        }
    }
    
    free_cycle_info(info);
    return has_cycle;
}

int handle_validate_cycle(char* args) {
    int vertex_index, channel_index, axis_number;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d %d", &vertex_index, &channel_index, &axis_number);
    if (parsed != 3) {
        print_argument_error("validate-cycle", "<vertex_index> <channel_index> <axis_number>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (!validate_vertex(vertex_index)) {
        printf("Error: vertex %d does not exist\n", vertex_index);
        return CMD_ERROR;
    }
    
    bool has_cycle = validate_cycle(vertex_index, channel_index, axis_number);
    printf("Path from vertex %d, channel %d, axis %d %s a cycle\n",
           vertex_index, channel_index, axis_number,
           has_cycle ? "forms" : "does not form");
           
    return CMD_SUCCESS;
}

int handle_print_cycle(char* args) {
    int vertex_index, channel_index, axis_number;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d %d", &vertex_index, &channel_index, &axis_number);
    if (parsed != 3) {
        print_argument_error("print-cycle", "<vertex_index> <channel_index> <axis_number>", false);
        return CMD_ERROR;
    }
    
    // Get cycle information
    cycleInfo* info = get_cycle_info(vertex_index, channel_index, axis_number);
    
    if (info->count == 0) {
        printf("No cycle found starting from vertex %d, channel %d, axis %d\n",
               vertex_index, channel_index, axis_number);
    } else {
        printf("Found cycle with %d vertices:\n", info->count);
        printf("Path: ");
        for (int i = 0; i < info->count; i++) {
            printf("(vertex %u, Ch %u)", info->vertices[i], info->channels[i]);
            if (i < info->count - 1) {
                printf(" -> ");
            }
        }
        printf(" -> (vertex %u, Ch %u)\n", info->vertices[0], info->channels[0]);
    }
    
    free_cycle_info(info);
    return CMD_SUCCESS;
}
int handle_create_sentence_from_string(char* args) {
    if (!args || !*args) {
        print_argument_error("create-sentence-str", "<text>", false);
        return ERROR;
    }

    // Count characters and allocate token array
    uint* tokens = malloc(MAX_SENTENCE_TOKENS * sizeof(uint));
    if (!tokens) {
        printf("Error: Failed to allocate memory\n");
        return ERROR;
    }
    int count = 0;

    // Convert each character to token vertex index (0-255)
    for (int i = 0; args[i] != '\0' && count < MAX_SENTENCE_TOKENS; i++) {
        // Each character's ASCII value maps directly to a token vertex
        uint token_vertex = (unsigned char)args[i];
        printf("token_vertex: %u\n", token_vertex);
        if (token_vertex > 255) {
            printf("Error: Invalid character (outside ASCII range)\n");
            free(tokens);
            return ERROR;
        }
        
        tokens[count++] = token_vertex;
    }

    if (count < 2) {
        printf("Error: At least 2 characters required for a sentence\n");
        free(tokens);
        return ERROR;
    }

    // Create sentence cycle from token vertices
    int result = create_sentence_cycle(tokens, count);
    free(tokens);

    if (result == LINK_SUCCESS) {
        printf("Successfully created sentence with %d characters\n", count);
        return SUCCESS;
    }

    printf("Error: Failed to create sentence cycle\n");
    return ERROR;
}

// Insert a path into an existing cycle at specified position
int insert_path_into_cycle(uint insert_vertex, ushort insert_channel, 
                          uint* path_vertices, ushort* path_channels, int path_length,
                          ushort axis_number) {
    if (!path_vertices || !path_channels || path_length < 1) {
        printf("Error: Invalid path data\n");
        return LINK_ERROR;
    }

    // Get cycle info at insertion point
    cycleInfo* cycle = get_cycle_info(insert_vertex, insert_channel, axis_number);
    if (!cycle || cycle->count < 2) {
        printf("Error: No valid cycle found at insertion point\n");
        if (cycle) free_cycle_info(cycle);
        return LINK_ERROR;
    }

    // Find insertion position in cycle
    int insert_pos = -1;
    for (int i = 0; i < cycle->count; i++) {
        if (cycle->vertices[i] == insert_vertex && 
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
    uint next_vertex = cycle->vertices[(insert_pos + 1) % cycle->count];
    ushort next_channel = cycle->channels[(insert_pos + 1) % cycle->count];
    
    // Remove link at insertion point
    if (delete_link(insert_vertex, insert_channel, next_vertex, next_channel, axis_number) != LINK_SUCCESS) {
        printf("Error: Failed to break cycle at insertion point\n");
        free_cycle_info(cycle);
        return LINK_ERROR;
    }

    // Link path start to insertion point
    if (create_link(insert_vertex, insert_channel,
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

    // Link path end to next vertex in original cycle
    if (create_link(path_vertices[path_length - 1], path_channels[path_length - 1],
                   next_vertex, next_channel,
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
    uint insert_vertex;
    ushort insert_channel, axis_number;
    uint path_vertices[MAX_cycle_vertices];
    ushort path_channels[MAX_cycle_vertices];
    int path_length = 0;

    // Parse insertion point and axis
    char* token = strtok(args, " ");
    if (!token || sscanf(token, "%u", &insert_vertex) != 1) {
        print_argument_error("insert-path", "<vertex> <channel> <axis> <path_vertices_and_channels...>", false);
        return CMD_ERROR;
    }

    token = strtok(NULL, " ");
    if (!token || sscanf(token, "%hu", &insert_channel) != 1) {
        print_argument_error("insert-path", "<vertex> <channel> <axis> <path_vertices_and_channels...>", false);
        return CMD_ERROR;
    }

    token = strtok(NULL, " ");
    if (!token || sscanf(token, "%hu", &axis_number) != 1) {
        print_argument_error("insert-path", "<vertex> <channel> <axis> <path_vertices_and_channels...>", false);
        return CMD_ERROR;
    }

    // Parse path vertices and channels
    while ((token = strtok(NULL, " ")) != NULL && path_length < MAX_cycle_vertices) {
        if (sscanf(token, "%u", &path_vertices[path_length]) != 1) {
            printf("Error: Invalid vertex index in path\n");
            return CMD_ERROR;
        }

        token = strtok(NULL, " ");
        if (!token || sscanf(token, "%hu", &path_channels[path_length]) != 1) {
            printf("Error: Missing channel for vertex %u\n", path_vertices[path_length]);
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
        uint vertex_position = get_vertex_position(path_vertices[i]);
        if (!Core[vertex_position]) {
            printf("Error: Vertex %u not loaded\n", path_vertices[i]);
            return CMD_ERROR;
        }
        
        ushort channel_count = get_channel_count(Core[vertex_position]);
        if (path_channels[i] >= channel_count) {
            printf("Error: Channel %hu does not exist in vertex %u (max: %hu)\n", 
                   path_channels[i], path_vertices[i], (ushort)(channel_count - 1));
            return CMD_ERROR;
        }
    }

    // Insert path into cycle
    if (insert_path_into_cycle(insert_vertex, insert_channel,
                              path_vertices, path_channels, path_length,
                              axis_number) == LINK_SUCCESS) {
        printf("Successfully inserted path of length %d into cycle\n", path_length);
        return CMD_SUCCESS;
    }

    printf("Error: Failed to insert path into cycle\n");
    return CMD_ERROR;
}

// Delete a path from an existing cycle
int delete_path_from_cycle(uint start_vertex, ushort start_channel, 
                          int path_length, ushort axis_number) {
    if (path_length < 1) {
        printf("Error: Invalid path length\n");
        return LINK_ERROR;
    }

    // Get cycle info at deletion point
    cycleInfo* cycle = get_cycle_info(start_vertex, start_channel, axis_number);
    if (!cycle || cycle->count < path_length + 2) {
        printf("Error: No valid cycle found or cycle too short\n");
        if (cycle) free_cycle_info(cycle);
        return LINK_ERROR;
    }

    // Find start position in cycle
    int start_pos = -1;
    for (int i = 0; i < cycle->count; i++) {
        if (cycle->vertices[i] == start_vertex && 
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
    uint end_vertex = cycle->vertices[(start_pos + path_length) % cycle->count];
    ushort end_channel = cycle->channels[(start_pos + path_length) % cycle->count];
    uint next_vertex = cycle->vertices[(start_pos + path_length + 1) % cycle->count];
    ushort next_channel = cycle->channels[(start_pos + path_length + 1) % cycle->count];

    // Break connections at path boundaries
    if (delete_link(start_vertex, start_channel, 
                   cycle->vertices[(start_pos + 1) % cycle->count],
                   cycle->channels[(start_pos + 1) % cycle->count],
                   axis_number) != LINK_SUCCESS ||
        delete_link(end_vertex, end_channel,
                   next_vertex, next_channel,
                   axis_number) != LINK_SUCCESS) {
        printf("Error: Failed to break cycle at path boundaries\n");
        free_cycle_info(cycle);
        return LINK_ERROR;
    }

    // Create new link to close the cycle
    if (create_link(start_vertex, start_channel,
                   next_vertex, next_channel,
                   axis_number) != LINK_SUCCESS) {
        printf("Error: Failed to close cycle\n");
        free_cycle_info(cycle);
        return LINK_ERROR;
    }

    // Clear channels in removed path
    for (int i = 0; i < path_length; i++) {
        int pos = (start_pos + 1 + i) % cycle->count;
        if (clear_channel(cycle->vertices[pos], cycle->channels[pos]) != CHANNEL_SUCCESS) {
            printf("Warning: Failed to clear channel in removed path\n");
        }
    }

    free_cycle_info(cycle);
    return LINK_SUCCESS;
}

// Command handler for deleting path from cycle
int handle_delete_path(char* args) {
    uint start_vertex;
    ushort start_channel, axis_number;
    int path_length;

    // Parse arguments
    if (sscanf(args, "%u %hu %hu %d", &start_vertex, &start_channel, 
               &axis_number, &path_length) != 4) {
        print_argument_error("delete-path", 
                           "<vertex> <channel> <axis> <path_length>", false);
        return CMD_ERROR;
    }

    // Delete path from cycle
    if (delete_path_from_cycle(start_vertex, start_channel,
                              path_length, axis_number) == LINK_SUCCESS) {
        printf("Successfully deleted path of length %d from cycle\n", path_length);
        return CMD_SUCCESS;
    }

    printf("Error: Failed to delete path from cycle\n");
    return CMD_ERROR;
}
