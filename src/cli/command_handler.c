#include "command_handler.h"
#include "../axis.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int handle_create_axis(char* args) {
    int node_index, channel_index, axis_number;
    
    // Parse arguments
    if (sscanf(args, "%d %d %d", &node_index, &channel_index, &axis_number) != 3) {
        printf("Error: Invalid arguments\n");
        printf("Usage: create-axis <node_index> <channel_index> <axis_number>\n");
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

void print_help() {
    printf("\nAvailable commands:\n");
    printf("  create-axis <node> <channel> <axis>  Create a new axis\n");
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