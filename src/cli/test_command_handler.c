#include "test_command_handler.h"
#include "../tests/axis_tests.h"
#include "../tests/link_tests.h"
#include "../tests/channel_tests.h"
#include "../Graph_structure/node.h"
#include "../Graph_structure/cycle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        printf("Error: node index must be between 0 and 255\n");
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
        printf("Error: node index must be between 0 and 255\n");
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
        printf("Error: node index must be between 0 and 255\n");
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
        printf("Error: node index must be between 0 and 255\n");
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
        printf("Error: node index must be between 0 and 255\n");
        return CMD_ERROR;
    }
    
    int failed = test_sequential_channel_creation(node_index);
    return (failed == 0) ? CMD_SUCCESS : CMD_ERROR;
}

int handle_test_sequential_token_creation(char* args) {
    if (args) {
        print_argument_error("test-sequential-token", "", false);
        return CMD_ERROR;
    }
    
    printf("\nTesting sequential token creation...\n");
    int failed = 0;
    
    // Test string to build up gradually
    const char* test_str = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int test_len = strlen(test_str);
    
    // Start with 2 characters and gradually increase
    for (int len = 2; len <= test_len; len++) {
        char substr[27] = {0};  // Large enough for full test string + null
        strncpy(substr, test_str, len);
        
        printf("Creating string for: %s\n", substr);
        
        // Create string from substring
        uint start_node;
        ushort start_channel;
        int result = handle_create_string(substr, &start_node, &start_channel, false, false);
        if (result == CMD_ERROR) {
            printf("Error: Failed to create string for %s\n", substr);
            failed++;
        } else {
            printf("Successfully created string for %s\n", substr);
        }
    }
    
    printf("\nSequential token creation tests completed. Failed tests: %d\n", failed);
    return (failed == 0) ? CMD_SUCCESS : CMD_ERROR;
}

int handle_test_repeating_string(char* args) {
    if (args) {
        print_argument_error("test-repeating-string", "", false);
        return CMD_ERROR;
    }
    
    printf("\nTesting repeating string creation...\n");
    int failed = 0;
    
    // Test strings with different pattern lengths (1-10) and repetitions (2-10)
    for (int pattern_len = 1; pattern_len <= 10; pattern_len++) {
        printf("\nTesting patterns of length %d\n", pattern_len);
        
        // Create base pattern
        char pattern[11] = {0};  // Max 10 chars + null
        for (int i = 0; i < pattern_len; i++) {
            pattern[i] = 'A' + i;  // A, B, C, etc.
        }
        
        // Test different repetition counts
        for (int repeat = 2; repeat <= 10; repeat++) {
            // Create test string by repeating pattern
            char test_str[101] = {0};  // Max 100 chars + null
            for (int i = 0; i < repeat; i++) {
                strcat(test_str, pattern);
            }
            
            printf("Creating string for: %s (pattern '%s' repeated %d times)\n", 
                   test_str, pattern, repeat);
            
            // Try to create string
            uint start_node;
            ushort start_channel;
            int result = handle_create_string(test_str, &start_node, &start_channel, false, false);
            if (result == CMD_ERROR) {
                printf("Error: Failed to create string for pattern '%s' repeated %d times\n",
                       pattern, repeat);
                failed++;
            } else {
                printf("Successfully created string\n");
            }
        }
    }
    
    printf("\nRepeating string tests completed. Failed tests: %d\n", failed);
    return (failed == 0) ? CMD_SUCCESS : CMD_ERROR;
}
