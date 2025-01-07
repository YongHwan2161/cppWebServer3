#include "command_handler.h"
#include "../axis.h"
#include "../channel.h"
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
    }
}

// Modified command handlers
int handle_create_axis(char* args) {
    int node_index, channel_index, axis_number;
    
    // Check if arguments are provided
    if (!args) {
        print_argument_error("create-axis", "<node_index> <channel_index> <axis_number>", true);
        return CMD_ERROR;
    }
    
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
    
    // Check if arguments are provided
    if (!args) {
        print_argument_error("check-axis", "<node_index> <channel_index> <axis_number>", true);
        return CMD_ERROR;
    }
    
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
    
    // Get channel offset
    uint channel_offset = get_channel_offset(Core[node_index], channel_index);
    
    // Check if axis exists
    bool exists = has_axis(Core[node_index], channel_offset, axis_number);
    printf("Axis %d %s in node %d, channel %d\n",
           axis_number,
           exists ? "exists" : "does not exist",
           node_index,
           channel_index);
           
    return CMD_SUCCESS;
}

int handle_list_axes(char* args) {
    int node_index, channel_index;
    
    // Check if arguments are provided
    if (!args) {
        print_argument_error("list-axes", "<node_index> <channel_index>", true);
        return CMD_ERROR;
    }
    
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
    
    // Check if arguments are provided
    if (!args) {
        print_argument_error("delete-axis", "<node_index> <channel_index> <axis_number>", true);
        return CMD_ERROR;
    }
    
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

void print_help() {
    printf("\nAvailable commands:\n");
    printf("  create-axis <node> <channel> <axis>  Create a new axis\n");
    printf("  check-axis <node> <channel> <axis>   Check if specific axis exists\n");
    printf("  list-axes <node> <channel>           List all axes in channel\n");
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
    
    if (strcmp(cmd, "create-axis") == 0) {
        return handle_create_axis(args);
    }
    else if (strcmp(cmd, "check-axis") == 0) {
        return handle_check_axis(args);
    }
    else if (strcmp(cmd, "list-axes") == 0) {
        return handle_list_axes(args);
    }
    else if (strcmp(cmd, "help") == 0) {
        print_help();
        return CMD_SUCCESS;
    }
    else if (strcmp(cmd, "exit") == 0) {
        return CMD_EXIT;
    }
    else {
        printf("Unknown command. Type 'help' for available commands.\n");
        return CMD_ERROR;
    }
} 