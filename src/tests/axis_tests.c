#include "../axis.h"
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