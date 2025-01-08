#include "../axis.h"
#include "../free_space.h"
#include <stdio.h>

int test_axis_creation() {
    printf("\nTesting axis creation...\n");
    
    int test_node = 0;
    int test_channel = 0;
    int failed_tests = 0;
    
    // Test 1: Create forward link axis
    int result = create_axis(test_node, test_channel, AXIS_FORWARD);
    if (result == AXIS_SUCCESS) {
        printf("✓ Successfully created forward axis\n");
    } else {
        printf("✗ Failed to create forward axis\n");
        failed_tests++;
    }
    
    // Test 2: Create backward link axis
    result = create_axis(test_node, test_channel, AXIS_BACKWARD);
    if (result == AXIS_SUCCESS) {
        printf("✓ Successfully created backward axis\n");
    } else {
        printf("✗ Failed to create backward axis\n");
        failed_tests++;
    }
    
    // Test 3: Create time axis
    result = create_axis(test_node, test_channel, AXIS_TIME);
    if (result == AXIS_SUCCESS) {
        printf("✓ Successfully created time axis\n");
    } else {
        printf("✗ Failed to create time axis\n");
        failed_tests++;
    }
    
    // Test 4: Try to create duplicate axis (should show warning)
    printf("\nTesting duplicate axis creation (expect warning):\n");
    result = create_axis(test_node, test_channel, AXIS_FORWARD);
    if (result == AXIS_SUCCESS) {
        printf("✓ Duplicate axis handling works correctly\n");
    } else {
        printf("✗ Unexpected error in duplicate axis handling\n");
        failed_tests++;
    }
    
    // Test 5: Try to create axis in invalid node
    printf("\nTesting invalid node (expect error):\n");
    result = create_axis(256, test_channel, AXIS_FORWARD);
    if (result == AXIS_ERROR) {
        printf("✓ Invalid node handling works correctly\n");
    } else {
        printf("✗ Failed to handle invalid node properly\n");
        failed_tests++;
    }
    
    printf("\nAxis creation tests completed: %d failed\n", failed_tests);
    return failed_tests;
}

int test_resize_node_space() {
    printf("\nTesting resize_node_space functionality...\n");
    int failed_tests = 0;
    
    // Test 1: Create initial axes to generate free blocks
    printf("Setting up test environment...\n");
    int result = create_axis(0, 0, 1);
    result &= create_axis(0, 0, 2);
    result &= create_axis(0, 0, 3);
    
    if (result != AXIS_SUCCESS) {
        printf("✗ Failed to set up test environment\n");
        return 1;
    }
    
    // Verify initial free space state
    printf("\nChecking initial free space state...\n");
    if (free_space->count != 2) {
        printf("✗ Expected 2 free blocks, got %d\n", free_space->count);
        failed_tests++;
    } else {
        printf("✓ Correct number of initial free blocks\n");
    }
    
    // Test 2: Create axis in node 1 to trigger resize
    printf("\nTesting node resize with free block reuse...\n");
    result = create_axis(1, 0, 1);
    
    if (result != AXIS_SUCCESS) {
        printf("✗ Failed to create axis for resize test\n");
        failed_tests++;
    } else {
        printf("✓ Successfully created axis triggering resize\n");
    }
    
    // Verify final free space state
    printf("\nChecking final free space state...\n");
    if (free_space->count != 2) {
        printf("✗ Expected 2 free blocks after resize, got %d\n", free_space->count);
        failed_tests++;
    } else {
        // Verify block sizes
        int found_16byte_blocks = 0;
        for (uint i = 0; i < free_space->count; i++) {
            if (free_space->blocks[i].size == 16) {
                found_16byte_blocks++;
            }
        }
        
        if (found_16byte_blocks != 2) {
            printf("✗ Expected 2 16-byte blocks, found %d\n", found_16byte_blocks);
            failed_tests++;
        } else {
            printf("✓ Correct free block sizes after resize\n");
        }
    }
    
    printf("\nResize node space tests completed: %d failed\n", failed_tests);
    return failed_tests;
} 