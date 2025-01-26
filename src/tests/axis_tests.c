#include "../Graph_structure/axis.h"
#include "../free_space.h"
#include "../Graph_structure/channel.h"
#include "../map.h"
#include <stdio.h>

int test_axis_creation() {
    printf("\nTesting axis creation...\n");
    
    int test_node = 0;
    int test_channel = 0;
    int failed_tests = 0;
    
    // Test 1: Create forward link axis
    int result = create_axis(test_node, test_channel, 0, true);
    if (result == AXIS_SUCCESS) {
        printf("✓ Successfully created forward axis\n");
    } else {
        printf("✗ Failed to create forward axis\n");
        failed_tests++;
    }
    
    // Test 2: Create backward link axis
    result = create_axis(test_node, test_channel, 1, true);
    if (result == AXIS_SUCCESS) {
        printf("✓ Successfully created backward axis\n");
    } else {
        printf("✗ Failed to create backward axis\n");
        failed_tests++;
    }
    
    // Test 3: Create time axis
    result = create_axis(test_node, test_channel, 2, true);
    if (result == AXIS_SUCCESS) {
        printf("✓ Successfully created time axis\n");
    } else {
        printf("✗ Failed to create time axis\n");
        failed_tests++;
    }
    
    // Test 4: Try to create duplicate axis (should show warning)
    printf("\nTesting duplicate axis creation (expect warning):\n");
    result = create_axis(test_node, test_channel, 0, true);
    if (result == AXIS_SUCCESS) {
        printf("✓ Duplicate axis handling works correctly\n");
    } else {
        printf("✗ Unexpected error in duplicate axis handling\n");
        failed_tests++;
    }
    
    // Test 5: Try to create axis in invalid node
    printf("\nTesting invalid node (expect error):\n");
    result = create_axis(256, test_channel, 0, true);
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
    int result = create_axis(0, 0, 1, true);
    result &= create_axis(0, 0, 2, true);
    result &= create_axis(0, 0, 3, true);
    
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
    result = create_axis(1, 0, 1, true);
    
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

int test_axis_create_delete(int node_index, int channel_index, int max_axis) {
    printf("\nTesting axis creation and deletion (max axis: %d)...\n", max_axis);
    int failed_tests = 0;
    long node_position = get_node_position(node_index);
    if (node_position == -1) return 1;
    // Test 1: Create axes from 0 to max_axis
    printf("Creating axes 0 to %d...\n", max_axis);
    for (int i = 0; i <= max_axis; i++) {
        int result = create_axis(node_index, channel_index, i, true);
        if (result != AXIS_SUCCESS) {
            printf("✗ Failed to create axis %d\n", i);
            failed_tests++;
        }
    }
    
    // Verify all axes were created
    printf("\nVerifying created axes...\n");
    for (int i = 0; i <= max_axis; i++) {
        
        if (!has_axis(Core[node_position], channel_index, i)) {
            printf("✗ Axis %d not found after creation\n", i);
            failed_tests++;
        }
    }
    
    // Test 2: Delete all axes
    printf("\nDeleting all axes...\n");
    for (int i = max_axis; i >= 0; i--) {
        int result = delete_axis(node_index, channel_index, i, true);
        if (result != AXIS_SUCCESS) {
            printf("✗ Failed to delete axis %d\n", i);
            failed_tests++;
        }
    }
    
    // Verify all axes were deleted
    printf("\nVerifying axis deletion...\n");
    for (int i = 0; i <= max_axis; i++) {
        if (has_axis(Core[node_position], channel_index, i)) {
            printf("✗ Axis %d still exists after deletion\n", i);
            failed_tests++;
        }
    }
    
    printf("\nAxis creation/deletion tests completed: %d failed\n", failed_tests);
    return failed_tests;
}

int test_free_block_offsets() {
    printf("\nTesting free block offset uniqueness...\n");
    int failed_tests = 0;
    
    // Skip test if no free blocks
    if (free_space->count == 0) {
        printf("No free blocks to test\n");
        return 0;
    }
    
    printf("Checking %d free blocks for offset conflicts...\n", free_space->count);
    
    // Check each pair of blocks for offset conflicts
    for (uint i = 0; i < free_space->count; i++) {
        for (uint j = i + 1; j < free_space->count; j++) {
            // Check if blocks overlap
            long offset_i = free_space->blocks[i].offset;
            long offset_j = free_space->blocks[j].offset;
            uint size_i = free_space->blocks[i].size;
            
            if (offset_i == offset_j) {
                printf("✗ Conflict found: Blocks %d and %d have same offset 0x%08lX\n",
                       i, j, offset_i);
                failed_tests++;
                continue;
            }
            
            // Check if one block overlaps with another
            if ((offset_i < offset_j && offset_i + size_i > offset_j) ||
                (offset_j < offset_i && offset_j + free_space->blocks[j].size > offset_i)) {
                printf("✗ Overlap found between blocks %d and %d\n", i, j);
                printf("  Block %d: offset=0x%08lX, size=%u\n", i, offset_i, size_i);
                printf("  Block %d: offset=0x%08lX, size=%u\n", 
                       j, offset_j, free_space->blocks[j].size);
                failed_tests++;
            }
        }
    }
    
    if (failed_tests == 0) {
        printf("✓ All free block offsets are unique and non-overlapping\n");
    }
    
    printf("\nFree block offset tests completed: %d failed\n", failed_tests);
    return failed_tests;
} 