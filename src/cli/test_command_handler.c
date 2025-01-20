#include "test_command_handler.h"
#include "../tests/axis_tests.h"
#include "../tests/link_tests.h"
#include "../tests/channel_tests.h"
#include "../Graph_structure/vertex.h"
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
    
    // Run resize vertex space tests
    failed += test_resize_vertex_space();
    
    printf("\nAll tests completed. Total failed tests: %d\n", failed);
    return (failed == 0) ? CMD_SUCCESS : CMD_ERROR;
}

int handle_test_resize(char* args) {
    if (args) {
        print_argument_error("test-resize", "", false);
        return CMD_ERROR;
    }
    
    int failed = test_resize_vertex_space();
    return (failed == 0) ? CMD_SUCCESS : CMD_ERROR;
}

int handle_test_axis_create_delete(char* args) {
    int vertex_index, channel_index, max_axis;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d %d", &vertex_index, &channel_index, &max_axis);
    if (parsed != 3) {
        print_argument_error("test-axis-create-delete", 
            "<vertex_index> <channel_index> <max_axis>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (vertex_index < 0 || vertex_index >= 256) {
        printf("Error: vertex index must be between 0 and 255\n");
        return CMD_ERROR;
    }
    
    if (max_axis < 0) {
        printf("Error: Maximum axis number must be non-negative\n");
        return CMD_ERROR;
    }
    
    int failed = test_axis_create_delete(vertex_index, channel_index, max_axis);
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
    int vertex_index, channel_index, axis_number;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d %d", &vertex_index, &channel_index, &axis_number);
    if (parsed != 3) {
        print_argument_error("test-multiple-link", 
            "<vertex_index> <channel_index> <axis_number>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (vertex_index < 0 || vertex_index >= 256) {
        printf("Error: vertex index must be between 0 and 255\n");
        return CMD_ERROR;
    }
    
    int failed = test_multiple_link_creation(vertex_index, channel_index, axis_number);
    return (failed == 0) ? CMD_SUCCESS : CMD_ERROR;
}

int handle_test_create_delete_links(char* args) {
    int vertex_index, channel_index, axis_number;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d %d", &vertex_index, &channel_index, &axis_number);
    if (parsed != 3) {
        print_argument_error("test-create-delete-links", 
            "<vertex_index> <channel_index> <axis_number>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (vertex_index < 0 || vertex_index >= 256) {
        printf("Error: vertex index must be between 0 and 255\n");
        return CMD_ERROR;
    }
    
    int failed = test_create_delete_links(vertex_index, channel_index, axis_number);
    return (failed == 0) ? CMD_SUCCESS : CMD_ERROR;
}

int handle_test_multi_channel_links(char* args) {
    int vertex_index;
    
    // Parse arguments
    int parsed = sscanf(args, "%d", &vertex_index);
    if (parsed != 1) {
        print_argument_error("test-multi-channel-links", "<vertex_index>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (vertex_index < 0 || vertex_index >= 256) {
        printf("Error: vertex index must be between 0 and 255\n");
        return CMD_ERROR;
    }
    
    int failed = test_multi_channel_links(vertex_index);
    return (failed == 0) ? CMD_SUCCESS : CMD_ERROR;
}

int handle_test_channel_creation(char* args) {
    int vertex_index;
    
    // Parse arguments
    int parsed = sscanf(args, "%d", &vertex_index);
    if (parsed != 1) {
        print_argument_error("test-channel-creation", "<vertex_index>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (vertex_index < 0 || vertex_index >= 256) {
        printf("Error: vertex index must be between 0 and 255\n");
        return CMD_ERROR;
    }
    
    int failed = test_sequential_channel_creation(vertex_index);
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
        
        printf("Creating sentence for: %s\n", substr);
        
        // Create sentence from substring
        uint start_vertex;
        ushort start_channel;
        int result = handle_create_sentence(substr, &start_vertex, &start_channel);
        if (result == CMD_ERROR) {
            printf("Error: Failed to create sentence for %s\n", substr);
            failed++;
        } else {
            printf("Successfully created sentence for %s\n", substr);
        }
    }
    
    printf("\nSequential token creation tests completed. Failed tests: %d\n", failed);
    return (failed == 0) ? CMD_SUCCESS : CMD_ERROR;
}

int handle_test_repeating_sentence(char* args) {
    if (args) {
        print_argument_error("test-repeating-sentence", "", false);
        return CMD_ERROR;
    }
    
    printf("\nTesting repeating sentence creation...\n");
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
            
            printf("Creating sentence for: %s (pattern '%s' repeated %d times)\n", 
                   test_str, pattern, repeat);
            
            // Try to create sentence
            uint start_vertex;
            ushort start_channel;
            int result = handle_create_sentence(test_str, &start_vertex, &start_channel);
            if (result == CMD_ERROR) {
                printf("Error: Failed to create sentence for pattern '%s' repeated %d times\n",
                       pattern, repeat);
                failed++;
            } else {
                printf("Successfully created sentence\n");
            }
        }
    }
    
    printf("\nRepeating sentence tests completed. Failed tests: %d\n", failed);
    return (failed == 0) ? CMD_SUCCESS : CMD_ERROR;
}
