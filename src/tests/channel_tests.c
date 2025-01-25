#include "channel_tests.h"
#include "../Graph_structure/channel.h"
#include "../Graph_structure/axis.h"
#include "../map.h"
#include <stdio.h>

int test_sequential_channel_creation(uint node_index) {
    int failed = 0;
    printf("Testing sequential channel creation...\n");
    printf("Target node: %d\n", node_index);
    
    // Store initial state
    long node_position = get_node_position(node_index);
    uchar* node = Core[(unsigned int)node_position];
    ushort initial_channel_count = get_channel_count(node);
    
    // Create channels until we reach maximum (or fail)
    printf("Creating channels sequentially...\n");
    const int MAX_TEST_CHANNELS = 100;  // Reasonable test limit
    
    for (int i = initial_channel_count; i < MAX_TEST_CHANNELS; i++) {
        int result = create_channel(node_index);
        if (result != CHANNEL_SUCCESS) {
            printf("Failed to create channel %d\n", i);
            failed++;
            break;
        }
        
        // Verify channel count
        node_position = get_node_position(node_index);
        node = Core[node_position];  // Reload node pointer as it might have changed
        ushort current_count = get_channel_count(node);
        if (current_count != i + 1) {
            printf("Channel count mismatch after creation %d: expected %d, got %d\n",
                   i, i + 1, current_count);
            failed++;
        }
        
        // Verify channel offset
        uint channel_offset = get_channel_offset(node, i);
        if (channel_offset == 0 && i > 0) {
            printf("Invalid channel offset for channel %d\n", i);
            failed++;
        }
        
        // Verify axis count initialization
        ushort axis_count = get_axis_count(node, i);
        if (axis_count != 0) {
            printf("New channel %d has non-zero axis count: %d\n", i, axis_count);
            failed++;
        }
    }
    
    // Verify final state
    node_position = get_node_position(node_index);
    node = Core[(unsigned int)node_position];
    ushort final_count = get_channel_count(node);
    printf("Final channel count: %d\n", final_count);
    
    if (failed > 0) {
        printf("Channel creation test completed with %d failures\n", failed);
    } else {
        printf("Channel creation test completed successfully\n");
    }
    
    return failed;
} 