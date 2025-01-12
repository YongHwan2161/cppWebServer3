#include "test_command_handler.h"
#include "../tests/axis_tests.h"
#include "../tests/link_tests.h"
#include "../tests/channel_tests.h"
#include <stdio.h>
#include <stdlib.h>

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

int handle_test_create_delete_links(char* args) {
    int node_index, channel_index, axis_number;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d %d", &node_index, &channel_index, &axis_number);
    if (parsed != 3) {
        print_argument_error("test-create-delete-links", 
            "<node_index> <channel_index> <axis_number>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (node_index < 0 || node_index >= 256) {
        printf("Error: Node index must be between 0 and 255\n");
        return CMD_ERROR;
    }
    
    int failed = test_create_delete_links(node_index, channel_index, axis_number);
    return (failed == 0) ? CMD_SUCCESS : CMD_ERROR;
}

int handle_test_multi_channel_links(char* args) {
    int node_index;
    
    // Parse arguments
    int parsed = sscanf(args, "%d", &node_index);
    if (parsed != 1) {
        print_argument_error("test-multi-channel-links", "<node_index>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (node_index < 0 || node_index >= 256) {
        printf("Error: Node index must be between 0 and 255\n");
        return CMD_ERROR;
    }
    
    int failed = test_multi_channel_links(node_index);
    return (failed == 0) ? CMD_SUCCESS : CMD_ERROR;
}

int handle_test_channel_creation(char* args) {
    int node_index;
    
    // Parse arguments
    int parsed = sscanf(args, "%d", &node_index);
    if (parsed != 1) {
        print_argument_error("test-channel-creation", "<node_index>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (node_index < 0 || node_index >= 256) {
        printf("Error: Node index must be between 0 and 255\n");
        return CMD_ERROR;
    }
    
    int failed = test_sequential_channel_creation(node_index);
    return (failed == 0) ? CMD_SUCCESS : CMD_ERROR;
}
