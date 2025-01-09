#include "link_tests.h"
#include "../link.h"
#include "../channel.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int test_multiple_link_creation(uint source_node, ushort source_ch, ushort axis_number) {
    int failed = 0;
    srand(time(NULL));  // Initialize random seed
    
    printf("Testing multiple link creation...\n");
    printf("Source: Node %d, Channel %d, Axis %d\n", source_node, source_ch, axis_number);
    
    // Get initial link count
    uchar* node = Core[source_node];
    uint channel_offset = get_channel_offset(node, source_ch);
    if (!has_axis(node, channel_offset, axis_number)) {
        printf("Node %d, Channel %d does not have axis %d\n", source_node, source_ch, axis_number);
        int result = create_axis(source_node, source_ch, axis_number);
        node = Core[source_node];
        printf("Axis created: %d\n", result);
        if (result != AXIS_SUCCESS) {
            printf("Failed to create axis %d\n", axis_number);
            return -1;
        }
    } 
    uint axis_offset = get_axis_offset(node, source_ch, axis_number);
    ushort initial_link_count = *(ushort*)(node + channel_offset + axis_offset);
    
    // Create 100 random links
    for (int i = 0; i < 100; i++) {
        // Generate random destination
        uint dest_node = rand() % 256;
        ushort dest_ch = rand() % 5;  // Assuming max 5 channels
        
        // Create link
        int result = create_link(source_node, source_ch, dest_node, dest_ch, axis_number);
        if (result != LINK_SUCCESS) {
            printf("Failed to create link %d\n", i);
            failed++;
            continue;
        }
        
        // Verify link count
        node = Core[source_node];  // Reload node pointer as it might have changed
        channel_offset = get_channel_offset(node, source_ch);
        axis_offset = get_axis_offset(node, source_ch, axis_number);
        ushort current_link_count = *(ushort*)(node + channel_offset + axis_offset);
        
        if (current_link_count != initial_link_count + i + 1) {
            printf("Link count mismatch after link %d: expected %d, got %d\n",
                   i, initial_link_count + i + 1, current_link_count);
            failed++;
            continue;
        }
        
        // Verify last link data
        Link* last_link = (Link*)(node + channel_offset + axis_offset + 2 + 
                                 (current_link_count - 1) * 6);
        if (last_link->node != dest_node || last_link->channel != dest_ch) {
            printf("Link data mismatch at link %d\n", i);
            printf("Expected: node %d, channel %d\n", dest_node, dest_ch);
            printf("Got: node %d, channel %d\n", last_link->node, last_link->channel);
            failed++;
        }
    }
    
    printf("Link creation test completed: %d failures\n", failed);
    return failed;
} 