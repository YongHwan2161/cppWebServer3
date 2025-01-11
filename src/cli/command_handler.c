#include "command_handler.h"
#include "../axis.h"
#include "../channel.h"
#include "../link.h"
#include "../free_space.h"
#include "../tests/axis_tests.h"
#include "../tests/link_tests.h"
#include "../node.h"
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
    int node_index, channel_index, axis_number;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d %d", &node_index, &channel_index, &axis_number);
    if (parsed != 3) {
        print_argument_error("create-axis", "<node_index> <channel_index> <axis_number>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (node_index < 0 || node_index >= 256) {
        printf("Error: Node index must be between 0 and 255\n");
        return CMD_ERROR;
    }
    
    // Create the axis
    int result = create_axis(node_index, channel_index, axis_number);
    if (result == AXIS_SUCCESS) {
        printf("Successfully created axis %d in node %d, channel %d\n",
               axis_number, node_index, channel_index);
        return CMD_SUCCESS;
    } else {
        printf("Failed to create axis\n");
        return CMD_ERROR;
    }
}

int handle_check_axis(char* args) {
    int node_index, channel_index, axis_number;
    

    // Parse arguments
    int parsed = sscanf(args, "%d %d %d", &node_index, &channel_index, &axis_number);
    if (parsed != 3) {
        print_argument_error("check-axis", "<node_index> <channel_index> <axis_number>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (node_index < 0 || node_index >= 256) {
        printf("Error: Node index must be between 0 and 255\n");
        return CMD_ERROR;
    }
    
    // Get node and check if it exists
    if (!Core[node_index]) {
        printf("Error: Node %d does not exist\n", node_index);
        return CMD_ERROR;
    }
    
    // Check if axis exists
    bool exists = has_axis(Core[node_index], channel_index, axis_number);
    printf("Axis %d %s in node %d, channel %d\n",
           axis_number,
           exists ? "exists" : "does not exist",
           node_index,
           channel_index);
           
    return CMD_SUCCESS;
}

int handle_list_axes(char* args) {
    int node_index, channel_index;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d", &node_index, &channel_index);
    if (parsed != 2) {
        print_argument_error("list-axes", "<node_index> <channel_index>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (node_index < 0 || node_index >= 256) {
        printf("Error: Node index must be between 0 and 255\n");
        return CMD_ERROR;
    }
    
    // Get node and check if it exists
    if (!Core[node_index]) {
        printf("Error: Node %d does not exist\n", node_index);
        return CMD_ERROR;
    }
    
    // Get channel offset
    uint channel_offset = get_channel_offset(Core[node_index], channel_index);
    
    // Get axis count
    ushort axis_count = *(ushort*)(Core[node_index] + channel_offset);
    
    printf("\nAxes in node %d, channel %d:\n", node_index, channel_index);
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
        axis_numbers[i] = *(ushort*)(Core[node_index] + axis_data_offset + (i * 6));
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
    int node_index, channel_index, axis_number;
    // Parse arguments
    int parsed = sscanf(args, "%d %d %d", &node_index, &channel_index, &axis_number);
    if (parsed != 3) {
        print_argument_error("delete-axis", "<node_index> <channel_index> <axis_number>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (node_index < 0 || node_index >= 256) {
        printf("Error: Node index must be between 0 and 255\n");
        return CMD_ERROR;
    }
    
    // Delete the axis
    int result = delete_axis(node_index, channel_index, axis_number);
    return (result == AXIS_SUCCESS) ? CMD_SUCCESS : CMD_ERROR;
}

int handle_create_link(char* args) {
    int source_node, source_ch, dest_node, dest_ch, axis_number;
    // Parse arguments
    int parsed = sscanf(args, "%d %d %d %d %d", 
                       &source_node, &source_ch, 
                       &dest_node, &dest_ch, 
                       &axis_number);
    if (parsed != 5) {
        print_argument_error("create-link", 
            "<source_node> <source_ch> <dest_node> <dest_ch> <axis_number>", 
            false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (!validate_node(source_node) ||
        !validate_node(dest_node)) {
        return CMD_ERROR;
    }
    int result = ensure_axis_exists(source_node, source_ch, axis_number);
    if (result != AXIS_SUCCESS) {
        return CMD_ERROR;
    }
    // Create the link
    result = create_link(source_node, source_ch, 
                           dest_node, dest_ch, 
                           axis_number);
    return (result == LINK_SUCCESS) ? CMD_SUCCESS : CMD_ERROR;
}

int handle_delete_link(char* args) {
    int source_node, source_ch, dest_node, dest_ch, axis_number;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d %d %d %d", 
                       &source_node, &source_ch, 
                       &dest_node, &dest_ch, 
                       &axis_number);
    if (parsed != 5) {
        print_argument_error("delete-link", 
            "<source_node> <source_ch> <dest_node> <dest_ch> <axis_number>", 
            false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (!validate_node(source_node) ||
        !validate_node(dest_node)) {
        return CMD_ERROR;
    }
    if (!has_axis(Core[source_node], source_ch, axis_number)) {
        printf("Error: Axis %d does not exist in node %d, channel %d\n",
               axis_number, source_node, source_ch);
        return CMD_ERROR;
    }
    
    // Delete the link
    int result = delete_link(source_node, source_ch, 
                           dest_node, dest_ch, 
                           axis_number);
    return (result == LINK_SUCCESS) ? CMD_SUCCESS : CMD_ERROR;
}

int handle_print_node(char* args) {
    int node_index;
    
    // Parse arguments
    int parsed = sscanf(args, "%d", &node_index);
    if (parsed != 1) {
        print_argument_error("print-node", "<node_index>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (node_index < 0 || node_index >= 256) {
        printf("Error: Node index must be between 0 and 255\n");
        return CMD_ERROR;
    }
    
    // Check if node exists
    if (!Core[node_index]) {
        printf("Error: Node %d does not exist\n", node_index);
        return CMD_ERROR;
    }
        // Print raw memory contents
    printf("\nMemory Contents:\n");
    printf("Offset    00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F    ASCII\n");
    printf("--------  -----------------------------------------------    ----------------\n");
    
    ushort node_size = 1 << (*(ushort*)Core[node_index]);
    for (int i = 0; i < node_size; i += 16) {
        // Print offset
        printf("%08X  ", i);
        
        // Print hex values
        for (int j = 0; j < 16; j++) {
            if (i + j < node_size) {
                printf("%02X ", Core[node_index][i + j]);
            } else {
                printf("   ");
            }
        }
        
        // Print ASCII representation
        printf("   ");
        for (int j = 0; j < 16 && i + j < node_size; j++) {
            char c = Core[node_index][i + j];
            printf("%c", (c >= 32 && c <= 126) ? c : '.');
        }
        printf("\n");
    }
    // Print node metadata
    printf("\nNode %d Information:\n", node_index);
    printf("Size: %d bytes\n", 1 << (*(ushort*)Core[node_index]));
    printf("Actual Size: %d bytes\n", *(uint*)(Core[node_index] + 2));
    printf("Core Position: %d\n", CoreMap[node_index].core_position);
    printf("File Offset: 0x%08lX\n", CoreMap[node_index].file_offset);
    printf("Load Status: %s\n", CoreMap[node_index].is_loaded ? "Loaded" : "Not loaded");
    
    // Get channel count
    ushort channel_count = get_channel_count(Core[node_index]);
    printf("\nChannel Count: %d\n", channel_count);
    
    // Print channel information
    for (int ch = 0; ch < channel_count; ch++) {
        uint channel_offset = get_channel_offset(Core[node_index], ch);
        printf("\nChannel %d (offset: 0x%04X):\n", ch, channel_offset);
        
        // Get axis count for this channel
        ushort axis_count = *(ushort*)(Core[node_index] + channel_offset);
        printf("  Axis Count: %d\n", axis_count);
        
        // Print axis information
        for (int i = 0; i < axis_count; i++) {
            // Get axis entry
            int axis_entry_offset = channel_offset + 2 + (i * 6);
            ushort axis_number = *(ushort*)(Core[node_index] + axis_entry_offset);
            uint axis_offset = *(uint*)(Core[node_index] + axis_entry_offset + 2);
            
            // Get axis type label
            const char* axis_type = "";
            switch(axis_number) {
                case AXIS_FORWARD: axis_type = "(Forward link)"; break;
                case AXIS_BACKWARD: axis_type = "(Backward link)"; break;
                case AXIS_TIME: axis_type = "(Time axis)"; break;
            }
            
            printf("  Axis %d %s (offset: 0x%04X):\n", axis_number, axis_type, axis_offset);
            
            // Get link count for this axis
            ushort link_count = *(ushort*)(Core[node_index] + channel_offset + axis_offset);
            printf("    Link Count: %d\n", link_count);
            
            // Print link information
            for (int j = 0; j < link_count; j++) {
                int link_offset = channel_offset + axis_offset + 2 + (j * 6);
                if (link_offset >= 1 << (*(ushort*)Core[node_index])) {
                    printf("    error: link offset %d is greater than node size %d\n", link_offset, 1 << (*(ushort*)Core[node_index]));
                    break;
                }
                uint dest_node = *(uint*)(Core[node_index] + link_offset);
                ushort dest_channel = *(ushort*)(Core[node_index] + link_offset + 4);
                printf("    Link %d: Node %d, Channel %d\n", 
                       j, dest_node, dest_channel);
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
    printf("Free node indices: %d\n", free_space->index_count);
    
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
        printf("\nFree Node Indices:\n");
        for (uint i = 0; i < free_space->index_count; i++) {
            printf("%d ", free_space->free_indices[i]);
        }
        printf("\n");
    }
    
    return CMD_SUCCESS;
}

int handle_run_tests(char* args) {
    if (args) {
        print_argument_error("run-tests", "", false);
        return CMD_ERROR;
    }
    
    printf("\nRunning all tests...\n");
    int failed = 0;
    
    // Run axis creation tests
    failed += test_axis_creation();
    
    // Run resize node space tests
    failed += test_resize_node_space();
    
    printf("\nAll tests completed. Total failed tests: %d\n", failed);
    return (failed == 0) ? CMD_SUCCESS : CMD_ERROR;
}

int handle_test_resize(char* args) {
    if (args) {
        print_argument_error("test-resize", "", false);
        return CMD_ERROR;
    }
    
    int failed = test_resize_node_space();
    return (failed == 0) ? CMD_SUCCESS : CMD_ERROR;
}

int handle_test_axis_create_delete(char* args) {
    int node_index, channel_index, max_axis;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d %d", &node_index, &channel_index, &max_axis);
    if (parsed != 3) {
        print_argument_error("test-axis-create-delete", 
            "<node_index> <channel_index> <max_axis>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (node_index < 0 || node_index >= 256) {
        printf("Error: Node index must be between 0 and 255\n");
        return CMD_ERROR;
    }
    
    if (max_axis < 0) {
        printf("Error: Maximum axis number must be non-negative\n");
        return CMD_ERROR;
    }
    
    int failed = test_axis_create_delete(node_index, channel_index, max_axis);
    return (failed == 0) ? CMD_SUCCESS : CMD_ERROR;
}

int handle_test_free_offsets(char* args) {
    if (args) {
        print_argument_error("test-free-offsets", "", false);
        return CMD_ERROR;
    }
    
    int failed = test_free_block_offsets();
    return (failed == 0) ? CMD_SUCCESS : CMD_ERROR;
}

int handle_test_multiple_link_creation(char* args) {
    int node_index, channel_index, axis_number;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d %d", &node_index, &channel_index, &axis_number);
    if (parsed != 3) {
        print_argument_error("test-multiple-link", 
            "<node_index> <channel_index> <axis_number>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (node_index < 0 || node_index >= 256) {
        printf("Error: Node index must be between 0 and 255\n");
        return CMD_ERROR;
    }
    
    int failed = test_multiple_link_creation(node_index, channel_index, axis_number);
    return (failed == 0) ? CMD_SUCCESS : CMD_ERROR;
}

void print_help() {
    printf("\nAvailable commands:\n");
    printf("  create-axis <node> <channel> <axis>  Create a new axis\n");
    printf("  check-axis <node> <channel> <axis>   Check if specific axis exists\n");
    printf("  list-axes <node> <channel>           List all axes in channel\n");
    printf("  delete-axis <node> <channel> <axis>  Delete an existing axis\n");
    printf("  create-link <src_node> <src_ch> <dst_node> <dst_ch> <axis>  Create a link\n");
    printf("  delete-link <src_node> <src_ch> <dst_node> <dst_ch> <axis>  Delete a link\n");
    printf("  print-node <node_index>               Print node data in hexadecimal format\n");
    printf("  print-free-space                     Print free space information\n");
    printf("  run-tests                           Run all test cases\n");
    printf("  test-resize                         Run resize node space tests\n");
    printf("  test-axis-create-delete <node> <ch> <max>  Test axis creation/deletion\n");
    printf("  test-free-offsets                    Test free block offset uniqueness\n");
    printf("  test-multiple-link <node> <ch> <axis>  Test multiple link creation\n");
    printf("  help                                 Show this help message\n");
    printf("  exit                                 Exit the program\n");
    printf("\nAxis types:\n");
    printf("  0: Forward link\n");
    printf("  1: Backward link\n");
    printf("  3: Time axis\n\n");
}

int handle_command(char* command) {
    char* cmd = strtok(command, " \n");
    if (!cmd) return CMD_SUCCESS;
    
    char* args = strtok(NULL, "\n");
    
    // Common argument validation
    if (strcmp(cmd, "help") == 0 || strcmp(cmd, "exit") == 0 || 
        strcmp(cmd, "print-free-space") == 0 || strcmp(cmd, "run-tests") == 0 ||
        strcmp(cmd, "test-resize") == 0 || strcmp(cmd, "test-free-offsets") == 0) {
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
        else if (strcmp(cmd, "test-free-offsets") == 0) {
            return handle_test_free_offsets(args);
        }
        return CMD_EXIT;
    }
    
    // Check if arguments are required but missing
    if (!args) {
        if (strcmp(cmd, "create-axis") == 0) {
            print_argument_error(cmd, "<node_index> <channel_index> <axis_number>", true);
        }
        else if (strcmp(cmd, "check-axis") == 0) {
            print_argument_error(cmd, "<node_index> <channel_index> <axis_number>", true);
        }
        else if (strcmp(cmd, "list-axes") == 0) {
            print_argument_error(cmd, "<node_index> <channel_index>", true);
        }
        else if (strcmp(cmd, "delete-axis") == 0) {
            print_argument_error(cmd, "<node_index> <channel_index> <axis_number>", true);
        }
        else if (strcmp(cmd, "create-link") == 0) {
            print_argument_error(cmd, "<source_node> <source_ch> <dest_node> <dest_ch> <axis_number>", true);
        }
        else if (strcmp(cmd, "print-node") == 0) {
            print_argument_error(cmd, "<node_index>", true);
        }
        else if (strcmp(cmd, "delete-link") == 0) {
            print_argument_error(cmd, 
                "<source_node> <source_ch> <dest_node> <dest_ch> <axis_number>", 
                true);
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
    else if (strcmp(cmd, "print-node") == 0) {
        return handle_print_node(args);
    }
    else if (strcmp(cmd, "delete-link") == 0) {
        return handle_delete_link(args);
    }
    else if (strcmp(cmd, "test-axis-create-delete") == 0) {
        return handle_test_axis_create_delete(args);
    }
    else if (strcmp(cmd, "test-free-offsets") == 0) {
        return handle_test_free_offsets(args);
    }
    else if (strcmp(cmd, "test-multiple-link") == 0) {
        return handle_test_multiple_link_creation(args);
    }
    else {
        printf("Unknown command. Type 'help' for available commands.\n");
        return CMD_ERROR;
    }
} 