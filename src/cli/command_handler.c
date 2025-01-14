#include "../Graph_structure/axis.h"
#include "../Graph_structure/channel.h"
#include "../Graph_structure/link.h"
#include "../free_space.h"
#include "../tests/axis_tests.h"
#include "../tests/link_tests.h"
#include "../tests/channel_tests.h"
#include "../Graph_structure/vertex.h"
#include "../map.h"
#include "../memory.h"
#include "command_handler.h"
#include "test_command_handler.h"
#include "validate_command_handler.h"
#include "../Graph_structure/circle.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Common error handling utilities
void print_command_usage(const char* command, const char* args_desc) {
    printf("Usage: %s %s\n", command, args_desc);
    printf("Example: %s\n", command);
}

void print_argument_error(const char* command, const char* args_desc, bool is_missing) {
    printf("Error: %s arguments\n", is_missing ? "Missing" : "Invalid");
    print_command_usage(command, args_desc);
    
    // Add example based on command type
    if (strcmp(command, "create-axis") == 0) {
        printf("Example: create-axis 0 0 0\n");
    } else if (strcmp(command, "check-axis") == 0) {
        printf("Example: check-axis 0 0 0\n");
    } else if (strcmp(command, "list-axes") == 0) {
        printf("Example: list-axes 0 0\n");
    } else if (strcmp(command, "delete-axis") == 0) {
        printf("Example: delete-axis 0 0 0\n");
    } else if (strcmp(command, "create-link") == 0) {
        printf("Example: create-link 0 0 1 0 0\n");
    }
}

// Modified command handlers
int handle_create_axis(char* args) {
    int vertex_index, channel_index, axis_number;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d %d", &vertex_index, &channel_index, &axis_number);
    if (parsed != 3) {
        print_argument_error("create-axis", "<vertex_index> <channel_index> <axis_number>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (vertex_index < 0 || vertex_index >= 256) {
        printf("Error: vertex index must be between 0 and 255\n");
        return CMD_ERROR;
    }
    
    // Create the axis
    int result = create_axis(vertex_index, channel_index, axis_number);
    if (result == AXIS_SUCCESS) {
        printf("Successfully created axis %d in vertex %d, channel %d\n",
               axis_number, vertex_index, channel_index);
        return CMD_SUCCESS;
    } else {
        printf("Failed to create axis\n");
        return CMD_ERROR;
    }
}

int handle_check_axis(char* args) {
    int vertex_index, channel_index, axis_number;
    

    // Parse arguments
    int parsed = sscanf(args, "%d %d %d", &vertex_index, &channel_index, &axis_number);
    if (parsed != 3) {
        print_argument_error("check-axis", "<vertex_index> <channel_index> <axis_number>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (vertex_index < 0 || vertex_index >= 256) {
        printf("Error: vertex index must be between 0 and 255\n");
        return CMD_ERROR;
    }
    
    // Get vertex and check if it exists
    if (!Core[vertex_index]) {
        printf("Error: vertex %d does not exist\n", vertex_index);
        return CMD_ERROR;
    }
    
    // Check if axis exists
    bool exists = has_axis(Core[vertex_index], channel_index, axis_number);
    printf("Axis %d %s in vertex %d, channel %d\n",
           axis_number,
           exists ? "exists" : "does not exist",
           vertex_index,
           channel_index);
           
    return CMD_SUCCESS;
}

int handle_list_axes(char* args) {
    int vertex_index, channel_index;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d", &vertex_index, &channel_index);
    if (parsed != 2) {
        print_argument_error("list-axes", "<vertex_index> <channel_index>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (vertex_index < 0 || vertex_index >= 256) {
        printf("Error: vertex index must be between 0 and 255\n");
        return CMD_ERROR;
    }
    
    // Get vertex and check if it exists
    if (!Core[vertex_index]) {
        printf("Error: vertex %d does not exist\n", vertex_index);
        return CMD_ERROR;
    }
    
    // Get channel offset
    uint channel_offset = get_channel_offset(Core[vertex_index], channel_index);
    
    // Get axis count
    ushort axis_count = *(ushort*)(Core[vertex_index] + channel_offset);
    
    printf("\nAxes in vertex %d, channel %d:\n", vertex_index, channel_index);
    if (axis_count == 0) {
        printf("No axes found\n");
        return CMD_SUCCESS;
    }
    
    printf("Total axes: %d\n", axis_count);
    
    // List all axes by scanning through axis data
    printf("\nAxis numbers:\n");
    int axis_data_offset = channel_offset + 2;  // Skip axis count
    
    // Create an array to store axis numbers for sorting (optional)
    ushort* axis_numbers = (ushort*)malloc(axis_count * sizeof(ushort));
    
    // Read all axis numbers
    for (int i = 0; i < axis_count; i++) {
        axis_numbers[i] = *(ushort*)(Core[vertex_index] + axis_data_offset + (i * 6));
    }
    
    // Optional: Sort axis numbers for better readability
    for (int i = 0; i < axis_count - 1; i++) {
        for (int j = 0; j < axis_count - i - 1; j++) {
            if (axis_numbers[j] > axis_numbers[j + 1]) {
                ushort temp = axis_numbers[j];
                axis_numbers[j] = axis_numbers[j + 1];
                axis_numbers[j + 1] = temp;
            }
        }
    }
    
    // Print axis numbers with special labels for known types
    for (int i = 0; i < axis_count; i++) {
        const char* axis_type = "";
        switch(axis_numbers[i]) {
            case AXIS_FORWARD:
                axis_type = "(Forward link)";
                break;
            case AXIS_BACKWARD:
                axis_type = "(Backward link)";
                break;
            case AXIS_TIME:
                axis_type = "(Time axis)";
                break;
        }
        printf("- Axis %d %s\n", axis_numbers[i], axis_type);
    }
    
    free(axis_numbers);
    return CMD_SUCCESS;
}

int handle_delete_axis(char* args) {
    int vertex_index, channel_index, axis_number;
    // Parse arguments
    int parsed = sscanf(args, "%d %d %d", &vertex_index, &channel_index, &axis_number);
    if (parsed != 3) {
        print_argument_error("delete-axis", "<vertex_index> <channel_index> <axis_number>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (vertex_index < 0 || vertex_index >= 256) {
        printf("Error: vertex index must be between 0 and 255\n");
        return CMD_ERROR;
    }
    
    // Delete the axis
    int result = delete_axis(vertex_index, channel_index, axis_number);
    return (result == AXIS_SUCCESS) ? CMD_SUCCESS : CMD_ERROR;
}

int handle_create_link(char* args) {
    int source_vertex, source_ch, dest_vertex, dest_ch, axis_number;
    // Parse arguments
    int parsed = sscanf(args, "%d %d %d %d %d", 
                       &source_vertex, &source_ch, 
                       &dest_vertex, &dest_ch, 
                       &axis_number);
    if (parsed != 5) {
        print_argument_error("create-link", 
            "<source_vertex> <source_ch> <dest_vertex> <dest_ch> <axis_number>", 
            false);
        return CMD_ERROR;
    }
    // Create the link
    int result = create_link(source_vertex, source_ch, 
                           dest_vertex, dest_ch, 
                           axis_number);
    return (result == LINK_SUCCESS) ? CMD_SUCCESS : CMD_ERROR;
}
int handle_create_loop(char* args) {
    int vertex_index, channel_index, axis_number;
    int parsed = sscanf(args, "%d %d %d", &vertex_index, &channel_index, &axis_number);
    if (parsed != 3) {
        print_argument_error("create-loop", "<vertex_index> <channel_index> <axis_number>", false);
        return CMD_ERROR;
    }    
    // Validate input
    if (!validate_vertex(vertex_index)) {
        return CMD_ERROR;
    }
    return create_loop(vertex_index, channel_index, axis_number);
}
int handle_create_bidirectional_link(char* args) {
    int source_vertex, source_ch, dest_vertex, dest_ch;
    int parsed = sscanf(args, "%d %d %d %d", 
                       &source_vertex, &source_ch, 
                       &dest_vertex, &dest_ch);
    if (parsed != 4) {
        print_argument_error("create-bidirectional-link", 
            "<source_vertex> <source_ch> <dest_vertex> <dest_ch>", 
            false);
        return CMD_ERROR;
    }
    return create_bidirectional_link(source_vertex, source_ch, dest_vertex, dest_ch);
}
int handle_delete_link(char* args) {
    int source_vertex, source_ch, dest_vertex, dest_ch, axis_number;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d %d %d %d", 
                       &source_vertex, &source_ch, 
                       &dest_vertex, &dest_ch, 
                       &axis_number);
    if (parsed != 5) {
        print_argument_error("delete-link", 
            "<source_vertex> <source_ch> <dest_vertex> <dest_ch> <axis_number>", 
            false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (!validate_vertex(source_vertex) ||
        !validate_vertex(dest_vertex)) {
        return CMD_ERROR;
    }
    if (!has_axis(Core[source_vertex], source_ch, axis_number)) {
        printf("Error: Axis %d does not exist in vertex %d, channel %d\n",
               axis_number, source_vertex, source_ch);
        return CMD_ERROR;
    }
    
    // Delete the link
    int result = delete_link(source_vertex, source_ch, 
                           dest_vertex, dest_ch, 
                           axis_number);
    return (result == LINK_SUCCESS) ? CMD_SUCCESS : CMD_ERROR;
}

int handle_print_vertex(char* args) {
    int vertex_index;
    
    // Parse arguments
    int parsed = sscanf(args, "%d", &vertex_index);
    if (parsed != 1) {
        print_argument_error("print-vertex", "<vertex_index>", false);
        return CMD_ERROR;
    }
    if (!CoreMap[vertex_index].is_loaded) {
        printf("Error: vertex %d is not loaded in memory\n", vertex_index);
        return CMD_ERROR;
    }
    uint vertex_position = get_vertex_position(vertex_index);
    printf("vertex %d is at Core position %d\n", vertex_index, vertex_position);
    // Check if vertex exists
    if (!Core[vertex_position]) {
        printf("Error: vertex %d does not exist\n", vertex_index);
        return CMD_ERROR;
    }
        // Print raw memory contents
    printf("\nMemory Contents:\n");
    printf("Offset    00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F    ASCII\n");
    printf("--------  -----------------------------------------------    ----------------\n");
    
    ushort vertex_size = 1 << (*(ushort*)Core[vertex_position]);
    printf("vertex_size: %d\n", vertex_size);
    for (int i = 0; i < vertex_size; i += 16) {
        // Print offset
        printf("%08X  ", i);
        
        // Print hex values
        for (int j = 0; j < 16; j++) {
            if (i + j < vertex_size) {
                printf("%02X ", Core[vertex_position][i + j]);
            } else {
                printf("   ");
            }
        }
        
        // Print ASCII representation
        printf("   ");
        for (int j = 0; j < 16 && i + j < vertex_size; j++) {
            char c = Core[vertex_index][i + j];
            printf("%c", (c >= 32 && c <= 126) ? c : '.');
        }
        printf("\n");
    }
    // Print vertex metadata
    printf("\nvertex %d Information:\n", vertex_index);
    printf("Size: %d bytes\n", 1 << (*(ushort*)Core[vertex_position]));
    printf("Actual Size: %d bytes\n", *(uint*)(Core[vertex_position] + 2));
    printf("Core Position: %d\n", CoreMap[vertex_index].core_position);
    printf("File Offset: 0x%08lX\n", CoreMap[vertex_index].file_offset);
    printf("Load Status: %s\n", CoreMap[vertex_index].is_loaded ? "Loaded" : "Not loaded");
    
    // Get channel count
    ushort channel_count = get_channel_count(Core[vertex_position]);
    printf("\nChannel Count: %d\n", channel_count);

    // Print channel information
    for (int ch = 0; ch < channel_count; ch++) {
        uint channel_offset = get_channel_offset(Core[vertex_position], ch);
        printf("\nChannel %d (offset: 0x%04X):\n", ch, channel_offset);
        
        // Get axis count for this channel
        ushort axis_count = *(ushort*)(Core[vertex_position] + channel_offset);
        printf("  Axis Count: %d\n", axis_count);
        
        // Print axis information
        for (int i = 0; i < axis_count; i++) {
            // Get axis entry
            int axis_entry_offset = channel_offset + 2 + (i * 6);
            ushort axis_number = *(ushort*)(Core[vertex_position] + axis_entry_offset);
            uint axis_offset = *(uint*)(Core[vertex_position] + axis_entry_offset + 2);
            
            // Get axis type label
            const char* axis_type = "";
            switch(axis_number) {
                case AXIS_FORWARD: axis_type = "(Forward link)"; break;
                case AXIS_BACKWARD: axis_type = "(Backward link)"; break;
                case AXIS_TIME: axis_type = "(Time axis)"; break;
            }
            
            printf("  Axis %d %s (offset: 0x%04X):\n", axis_number, axis_type, axis_offset);
            
            // Get link count for this axis
            ushort link_count = *(ushort*)(Core[vertex_position] + channel_offset + axis_offset);
            printf("    Link Count: %d\n", link_count);
            
            // Print link information
            for (int j = 0; j < link_count; j++) {
                int link_offset = channel_offset + axis_offset + 2 + (j * 6);
                if (link_offset >= 1 << (*(ushort*)Core[vertex_position])) {
                    printf("    error: link offset %d is greater than vertex size %d\n", link_offset, 1 << (*(ushort*)Core[vertex_position]));
                    break;
                }
                uint dest_vertex = *(uint*)(Core[vertex_position] + link_offset);
                ushort dest_channel = *(ushort*)(Core[vertex_position] + link_offset + 4);
                printf("    Link %d: vertex %d, Channel %d\n", 
                       j, dest_vertex, dest_channel);
            }
        }
    }
    
    
    return CMD_SUCCESS;
}

int handle_print_free_space(char* args) {
    if (args) {
        print_argument_error("print-free-space", "", false);
        return CMD_ERROR;
    }
    
    if (!free_space) {
        printf("Error: Free space manager not initialized\n");
        return CMD_ERROR;
    }
    
    printf("\nFree Space Information:\n");
    printf("Total free blocks: %d\n", free_space->count);
    printf("Free vertex indices: %d\n", free_space->index_count);
    
    if (free_space->count > 0) {
        printf("\nFree Blocks:\n");
        printf("Size (bytes)    Offset\n");
        printf("------------    ------\n");
        for (uint i = 0; i < free_space->count; i++) {
            printf("%-14u    0x%08lX\n", 
                   free_space->blocks[i].size,
                   free_space->blocks[i].offset);
        }
    }
    
    if (free_space->index_count > 0) {
        printf("\nFree vertex Indices:\n");
        for (uint i = 0; i < free_space->index_count; i++) {
            printf("%d ", free_space->free_indices[i]);
        }
        printf("\n");
    }
    
    return CMD_SUCCESS;
}

int handle_create_channel(char* args) {
    int vertex_index;
    
    // Parse arguments
    int parsed = sscanf(args, "%d", &vertex_index);
    if (parsed != 1) {
        print_argument_error("create-channel", "<vertex_index>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (!validate_vertex(vertex_index)) {
        printf("Error: vertex %d does not exist\n", vertex_index);
        return CMD_ERROR;
    }
    
    // Create the channel
    int result = create_channel(vertex_index);
    if (result == CHANNEL_SUCCESS) {
        printf("Successfully created new channel in vertex %d\n", vertex_index);
        return CMD_SUCCESS;
    } else {
        printf("Failed to create channel in vertex %d\n", vertex_index);
        return CMD_ERROR;
    }
}

int handle_clear_channel(char* args) {
    int vertex_index, channel_index;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d", &vertex_index, &channel_index);
    if (parsed != 2) {
        print_argument_error("clear-channel", "<vertex_index> <channel_index>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (!validate_vertex(vertex_index)) {
        printf("Error: vertex %d does not exist\n", vertex_index);
        return CMD_ERROR;
    }
    
    // Clear the channel
    int result = clear_channel(vertex_index, channel_index);
    if (result == CHANNEL_SUCCESS) {
        printf("Successfully cleared channel %d in vertex %d\n", channel_index, vertex_index);
        return CMD_SUCCESS;
    } else {
        printf("Failed to clear channel %d in vertex %d\n", channel_index, vertex_index);
        return CMD_ERROR;
    }
}

int handle_get_channel_offset(char* args) {
    int vertex_index, channel_index;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d", &vertex_index, &channel_index);
    if (parsed != 2) {
        print_argument_error("get-channel-offset", "<vertex_index> <channel_index>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (vertex_index < 0 || vertex_index >= 256) {
        printf("Error: vertex index must be between 0 and 255\n");
        return CMD_ERROR;
    }
    
    // Get vertex and check if it exists
    if (!Core[vertex_index]) {
        printf("Error: vertex %d does not exist\n", vertex_index);
        return CMD_ERROR;
    }
    
    // Get channel offset
    uint channel_offset = get_channel_offset(Core[vertex_index], channel_index);
    printf("Channel %d offset in vertex %d: 0x%04X\n", 
           channel_index, vertex_index, channel_offset);
           
    return CMD_SUCCESS;
}

int handle_get_vertex_position(char* args) {
    int vertex_index;
    
    // Parse arguments
    int parsed = sscanf(args, "%d", &vertex_index);
    if (parsed != 1) {
        print_argument_error("get-vertex-position", "<vertex_index>", false);
        return CMD_ERROR;
    }
    
    // Get vertex position
    int position = get_vertex_position(vertex_index);
    if (position >= 0) {
        printf("vertex %d is at Core position %d\n", vertex_index, position);
        printf("Memory address: %p\n", (void*)Core[position]);
        return CMD_SUCCESS;
    }
    
    return CMD_ERROR;
}

int handle_unload_vertex(char* args) {
    int vertex_index;
    
    // Parse arguments
    int parsed = sscanf(args, "%d", &vertex_index);
    if (parsed != 1) {
        print_argument_error("unload-vertex", "<vertex_index>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (vertex_index < 0 || vertex_index >= 256) {
        printf("Error: vertex index must be between 0 and 255\n");
        return CMD_ERROR;
    }
    
    // Unload the vertex
    if (unload_vertex_data(vertex_index)) {
        printf("Successfully unloaded vertex %d from memory\n", vertex_index);
        return CMD_SUCCESS;
    }
    
    return CMD_ERROR;
}

int handle_load_vertex(char* args) {
    int vertex_index;
    
    // Parse arguments
    int parsed = sscanf(args, "%d", &vertex_index);
    if (parsed != 1) {
        print_argument_error("load-vertex", "<vertex_index>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (vertex_index < 0) {
        printf("Error: vertex index must be between 0 and 255\n");
        return CMD_ERROR;
    }
    
    // Check if already loaded
    if (CoreMap[vertex_index].is_loaded) {
        printf("vertex %d is already loaded at Core position %d\n", 
               vertex_index, CoreMap[vertex_index].core_position);
        return CMD_ERROR;
    }
    
    // Load the vertex
    int position = load_vertex_to_core(vertex_index);
    if (position >= 0) {
        printf("Successfully loaded vertex %d to Core position %d\n", 
               vertex_index, position);
        return CMD_SUCCESS;
    }
    
    printf("Failed to load vertex %d\n", vertex_index);
    return CMD_ERROR;
}

int handle_print_coremap(char* args) {
    int vertex_index = -1;
    
    if (args) {
        // Parse vertex index
        if (sscanf(args, "%d", &vertex_index) != 1) {
            print_argument_error("print-coremap", "<vertex_index>", false);
            return CMD_ERROR;
        }
        
        // Validate vertex index
        if (vertex_index < 0 ) {
            printf("Error: vertex index must be between 0 and 255\n");
            return CMD_ERROR;
        }
        
        // Print single vertex info
        printf("\nCoreMap Status for vertex %d:\n", vertex_index);
        printf("%-8s %-15s %-15s %-15s\n", 
               "vertex", "Core Position", "Is Loaded", "File Offset");
        printf("--------------------------------------------------------\n");
        printf("%-8d %-15d %-15s 0x%08lX\n", 
               vertex_index,
               CoreMap[vertex_index].core_position,
               CoreMap[vertex_index].is_loaded ? "Yes" : "No",
               CoreMap[vertex_index].file_offset);
    } else {
        // Print summary of loaded vertices
        printf("\nCoreMap Status:\n");
        printf("Total Loaded vertices: %d\n\n", CoreSize);
        printf("%-8s %-15s %-15s %-15s\n", 
               "vertex", "Core Position", "Is Loaded", "File Offset");
        printf("--------------------------------------------------------\n");
        
        for (int i = 0; i < 256; i++) {
            if (CoreMap[i].is_loaded) {
                printf("%-8d %-15d %-15s 0x%08lX\n", 
                       i,
                       CoreMap[i].core_position,
                       "Yes",
                       CoreMap[i].file_offset);
            }
        }
    }
    printf("\n");
    return CMD_SUCCESS;
}

int handle_check_core_size() {
    printf("\nCore Memory Status:\n");
    printf("Current vertex Count: %d\n", CurrentvertexCount);
    printf("Current Core Size: %d\n", CoreSize);
    printf("Maximum Core Size: %d\n", MaxCoreSize);
    printf("Available Slots: %d\n", MaxCoreSize - CoreSize);
    
    // Show utilization percentage
    float utilization = ((float)CoreSize / MaxCoreSize) * 100;
    printf("Memory Utilization: %.1f%%\n\n", utilization);
    
    return CMD_SUCCESS;
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
    
    // Delete the vertex
    if (delete_vertex(vertex_index) == VERTEX_SUCCESS) {
        printf("Successfully deleted vertex %d\n", vertex_index);
    } else if (delete_vertex(vertex_index) == VERTEX_ERROR_GARBAGE) {
        printf("Error: Cannot delete garbage vertex (index %d)\n", GarbagevertexIndex);
    } else if (delete_vertex(vertex_index) == VERTEX_ERROR_IN_GARBAGE_CIRCLE) {
        printf("Error: Vertex %d is in garbage circle\n", vertex_index);
    } else {
        printf("Failed to delete vertex %d\n", vertex_index);
    }
    
    return CMD_SUCCESS;
}

void print_help() {
    printf("\nAvailable commands:\n");

    printf("  unload-vertex <vertex>                  Unload vertex from memory\n");
    printf("  load-vertex <vertex>                    Load vertex into memory\n");
    printf("  create-vertex                        Create a new vertex\n");

    printf("  create-axis <vertex> <channel> <axis>  Create a new axis\n");
    printf("  check-axis <vertex> <channel> <axis>   Check if specific axis exists\n");
    printf("  list-axes <vertex> <channel>           List all axes in channel\n");
    printf("  delete-axis <vertex> <channel> <axis>  Delete an existing axis\n");

    printf("  create-link <src_vertex> <src_ch> <dst_vertex> <dst_ch> <axis>  Create a link\n");
    printf("  create-loop <vertex> <ch> <axis>      Create a loop\n");
    printf("  delete-link <src_vertex> <src_ch> <dst_vertex> <dst_ch> <axis>  Delete a link\n");

    printf("  create-channel <vertex>               Create a new channel in vertex\n");
    printf("  clear-channel <vertex> <channel>        Clear all data in a channel\n");

    printf("  print-vertex <vertex_index>               Print vertex data in hexadecimal format\n");
    printf("  print-free-space                     Print free space information\n");
    printf("  get-channel-offset <vertex> <channel>    Get channel offset\n");
    printf("  get-vertex-position <vertex>             Get vertex's position in Core array\n");
    printf("  print-coremap [vertex_index]          Print CoreMap status (with optional vertex index)\n");
    printf("  check-core-size                    Show Core memory usage statistics\n");
    
    printf("  validate-circle <vertex> <ch> <axis>  Check if path forms a circle\n");
    printf("  print-circle <vertex> <ch> <axis>      Print circle information\n");

    printf("  run-tests                           Run all test cases\n");
    printf("  test-resize                         Run resize vertex space tests\n");
    printf("  test-axis-create-delete <vertex> <ch> <max>  Test axis creation/deletion\n");
    printf("  test-multiple-link <vertex> <ch> <axis>  Test multiple link creation\n");
    printf("  test-create-delete-links <vertex> <ch> <axis>  Test link creation/deletion cycle\n");
    printf("  test-multi-channel-links <vertex>      Test link creation/deletion across multiple channels\n");
    printf("  test-channel-creation <vertex>         Test sequential channel creation\n");
    
    printf("  help                                 Show this help message\n");
    printf("  exit                                 Exit the program\n");
    printf("  delete-vertex <vertex_index>            Delete a vertex and add to garbage chain\n");
    printf("\nAxis types:\n");
    printf("  0: Forward link\n");
    printf("  1: Backward link\n");
    printf("  3: Time axis\n\n");

    printf("  validate-free-offsets               Validate free block offsets\n");
    printf("  validate-circle <vertex> <ch> <axis>  Check if path forms a circle\n");
}

int handle_command(char* command) {
    char* cmd = strtok(command, " \n");
    if (!cmd) return CMD_SUCCESS;
    
    char* args = strtok(NULL, "\n");
    
    // Common argument validation
    if (strcmp(cmd, "help") == 0 || strcmp(cmd, "exit") == 0 || 
        strcmp(cmd, "print-free-space") == 0 || strcmp(cmd, "run-tests") == 0 ||
        strcmp(cmd, "test-resize") == 0 || strcmp(cmd, "validate-free-offsets") == 0 ||
        strcmp(cmd, "create-vertex") == 0) {
        // These commands don't need arguments
        if (strcmp(cmd, "help") == 0) {
            print_help();
            return CMD_SUCCESS;
        }
        else if (strcmp(cmd, "print-free-space") == 0) {
            return handle_print_free_space(args);
        }
        else if (strcmp(cmd, "run-tests") == 0) {
            return handle_run_tests(args);
        }
        else if (strcmp(cmd, "test-resize") == 0) {
            return handle_test_resize(args);
        }
        else if (strcmp(cmd, "validate-free-offsets") == 0) {
            return handle_validate_free_offsets(args);
        }
        else if (strcmp(cmd, "create-vertex") == 0) {
            return handle_create_vertex(args);
        }
        return CMD_EXIT;
    }
    
    // Check if arguments are required but missing
    if (!args) {
        if (strcmp(cmd, "create-axis") == 0) {
            print_argument_error(cmd, "<vertex_index> <channel_index> <axis_number>", true);
        }
        else if (strcmp(cmd, "check-axis") == 0) {
            print_argument_error(cmd, "<vertex_index> <channel_index> <axis_number>", true);
        }
        else if (strcmp(cmd, "list-axes") == 0) {
            print_argument_error(cmd, "<vertex_index> <channel_index>", true);
        }
        else if (strcmp(cmd, "delete-axis") == 0) {
            print_argument_error(cmd, "<vertex_index> <channel_index> <axis_number>", true);
        }
        else if (strcmp(cmd, "create-link") == 0) {
            print_argument_error(cmd, "<source_vertex> <source_ch> <dest_vertex> <dest_ch> <axis_number>", true);
        }
        else if (strcmp(cmd, "create-loop") == 0) {
            print_argument_error(cmd, "<vertex_index> <channel_index> <axis_number>", true);
        }
        else if (strcmp(cmd, "create-bidirectional-link") == 0) {
            print_argument_error(cmd, "<source_vertex> <source_ch> <dest_vertex> <dest_ch>", true);
        }
        else if (strcmp(cmd, "print-vertex") == 0) {
            print_argument_error(cmd, "<vertex_index>", true);
        }
        else if (strcmp(cmd, "delete-link") == 0) {
            print_argument_error(cmd, 
                "<source_vertex> <source_ch> <dest_vertex> <dest_ch> <axis_number>", 
                true);
        }
        else if (strcmp(cmd, "create-channel") == 0) {
            print_argument_error(cmd, "<vertex_index>", true);
        }
        else if (strcmp(cmd, "test-multi-channel-links") == 0) {
            print_argument_error(cmd, "<vertex_index>", true);
        }
        else if (strcmp(cmd, "test-channel-creation") == 0) {
            print_argument_error(cmd, "<vertex_index>", true);
        }
        else if (strcmp(cmd, "test-axis-create-delete") == 0) {
            print_argument_error(cmd, "<vertex_index> <channel_index> <max_axis>", true);
        }
        else if (strcmp(cmd, "clear-channel") == 0) {
            print_argument_error(cmd, "<vertex_index> <channel_index>", true);
        }
        else if (strcmp(cmd, "get-channel-offset") == 0) {
            print_argument_error(cmd, "<vertex_index> <channel_index>", true);
        }
        else if (strcmp(cmd, "get-vertex-position") == 0) {
            print_argument_error(cmd, "<vertex_index>", true);
        }
        else if (strcmp(cmd, "unload-vertex") == 0) {
            print_argument_error(cmd, "<vertex_index>", true);
        }
        else if (strcmp(cmd, "load-vertex") == 0) {
            print_argument_error(cmd, "<vertex_index>", true);
        }
        else if (strcmp(cmd, "print-coremap") == 0) {
            return handle_print_coremap(args);
        }
        else if (strcmp(cmd, "check-core-size") == 0) {
            return handle_check_core_size(args);
        }
        else if (strcmp(cmd, "delete-vertex") == 0) {
            return handle_delete_vertex(args);
        }
        else {
            printf("Unknown command. Type 'help' for available commands.\n");
        }
        return CMD_ERROR;
    }
    
    // Execute command with arguments
    if (strcmp(cmd, "create-axis") == 0) {
        return handle_create_axis(args);
    }
    else if (strcmp(cmd, "check-axis") == 0) {
        return handle_check_axis(args);
    }
    else if (strcmp(cmd, "list-axes") == 0) {
        return handle_list_axes(args);
    }
    else if (strcmp(cmd, "delete-axis") == 0) {
        return handle_delete_axis(args);
    }
    else if (strcmp(cmd, "create-link") == 0) {
        return handle_create_link(args);
    }
    else if (strcmp(cmd, "create-loop") == 0) {
        return handle_create_loop(args);
    }
    else if (strcmp(cmd, "create-bidirectional-link") == 0) {
        return handle_create_bidirectional_link(args);
    }
    else if (strcmp(cmd, "print-vertex") == 0) {
        return handle_print_vertex(args);
    }
    else if (strcmp(cmd, "delete-link") == 0) {
        return handle_delete_link(args);
    }
    else if (strcmp(cmd, "test-axis-create-delete") == 0) {
        return handle_test_axis_create_delete(args);
    }
    else if (strcmp(cmd, "test-multiple-link") == 0) {
        return handle_test_multiple_link_creation(args);
    }
    else if (strcmp(cmd, "test-create-delete-links") == 0) {
        return handle_test_create_delete_links(args);
    }
    else if (strcmp(cmd, "create-channel") == 0) {
        return handle_create_channel(args);
    }
    else if (strcmp(cmd, "test-multi-channel-links") == 0) {
        return handle_test_multi_channel_links(args);
    }
    else if (strcmp(cmd, "test-channel-creation") == 0) {
        return handle_test_channel_creation(args);
    }
    else if (strcmp(cmd, "clear-channel") == 0) {
        return handle_clear_channel(args);
    }
    else if (strcmp(cmd, "get-channel-offset") == 0) {
        return handle_get_channel_offset(args);
    }
    else if (strcmp(cmd, "get-vertex-position") == 0) {
        return handle_get_vertex_position(args);
    }
    else if (strcmp(cmd, "unload-vertex") == 0) {
        return handle_unload_vertex(args);
    }
    else if (strcmp(cmd, "load-vertex") == 0) {
        return handle_load_vertex(args);
    }
    else if (strcmp(cmd, "print-coremap") == 0) {
        return handle_print_coremap(args);
    }
    else if (strcmp(cmd, "check-core-size") == 0) {
        return handle_check_core_size(args);
    }
    else if (strcmp(cmd, "create-vertex") == 0) {
        return handle_create_vertex(args);
    }
    else if (strcmp(cmd, "delete-vertex") == 0) {
        return handle_delete_vertex(args);
    }
// Update handle_command to include new commands:
else if (strcmp(cmd, "validate-free-offsets") == 0) {
    return handle_validate_free_offsets(args);
    }
    else if (strcmp(cmd, "validate-circle") == 0) {
        return handle_validate_circle(args);
    } 
    else if (strcmp(cmd, "print-circle") == 0) {
        return handle_print_circle(args);
    }
    else {
        printf("Unknown command. Type 'help' for available commands.\n");
        return CMD_ERROR;
    }
}

int handle_validate_free_offsets(char* args) {
    if (args) {
        print_argument_error("validate-free-offsets", "", false);
        return CMD_ERROR;
    }
    return validate_free_offsets(args) ? CMD_SUCCESS : CMD_ERROR;
}

int handle_validate_circle(char* args) {
    int vertex_index, channel_index, axis_number;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d %d", &vertex_index, &channel_index, &axis_number);
    if (parsed != 3) {
        print_argument_error("validate-circle", "<vertex_index> <channel_index> <axis_number>", false);
        return CMD_ERROR;
    }    

    bool has_circle = validate_circle(vertex_index, channel_index, axis_number);
    printf("Path from vertex %d, channel %d, axis %d %s a circle\n",
           vertex_index, channel_index, axis_number,
           has_circle ? "forms" : "does not form");
           
    return CMD_SUCCESS;
}

int handle_print_circle(char* args) {
    int vertex_index, channel_index, axis_number;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d %d", &vertex_index, &channel_index, &axis_number);
    if (parsed != 3) {
        print_argument_error("print-circle", "<vertex_index> <channel_index> <axis_number>", false);
        return CMD_ERROR;
    }
    
    // Get circle information
    CircleInfo* info = get_circle_info(vertex_index, channel_index, axis_number);
    
    if (info->count == 0) {
        printf("No circle found starting from vertex %d, channel %d, axis %d\n",
               vertex_index, channel_index, axis_number);
    } else {
        printf("Found circle with %d vertices:\n", info->count);
        printf("Path: ");
        for (int i = 0; i < info->count; i++) {
            printf("(vertex %u, Ch %u)", info->vertices[i], info->channels[i]);
            if (i < info->count - 1) {
                printf(" -> ");
            }
        }
        printf(" -> (vertex %u, Ch %u)\n", info->vertices[0], info->channels[0]);
    }
    
    free_circle_info(info);
    return CMD_SUCCESS;
}
