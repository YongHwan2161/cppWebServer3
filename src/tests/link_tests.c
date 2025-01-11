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
    if (!ensure_axis_exists(source_node, source_ch, axis_number)) {
        printf("Failed to ensure axis %d exists\n", axis_number);
        return -1;
    }

    uchar* node = Core[source_node];
    uint channel_offset = get_channel_offset(node, source_ch);

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

int test_create_delete_links(uint source_node, ushort source_ch, ushort axis_number) {
    int failed = 0;
    printf("Testing link creation and deletion cycle...\n");
    printf("Source: Node %d, Channel %d, Axis %d\n", source_node, source_ch, axis_number);
    
    // Get initial link count
    if (!ensure_axis_exists(source_node, source_ch, axis_number)) {
        printf("Failed to ensure axis %d exists\n", axis_number);
        return -1;
    }

    uchar* node = Core[source_node];
    uint channel_offset = get_channel_offset(node, source_ch);
    uint axis_offset = get_axis_offset(node, source_ch, axis_number);
    ushort initial_link_count = *(ushort*)(node + channel_offset + axis_offset);
    
    // Store link destinations for later deletion
    uint dest_nodes[100];
    ushort dest_channels[100];
    
    // Create 100 sequential links
    printf("Creating 100 links...\n");
    for (int i = 0; i < 100; i++) {
        // Use sequential destination nodes and channels
        uint dest_node = (source_node + i + 1) % 256;  // Wrap around at 256
        ushort dest_ch = i % 5;  // Use channels 0-4
        
        // Store destinations for deletion
        dest_nodes[i] = dest_node;
        dest_channels[i] = dest_ch;
        
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
            printf("Link count mismatch after creation %d: expected %d, got %d\n",
                   i, initial_link_count + i + 1, current_link_count);
            failed++;
        }
    }
    
    // Delete all created links in reverse order
    printf("Deleting all links...\n");
    for (int i = 99; i >= 0; i--) {
        int result = delete_link(source_node, source_ch, dest_nodes[i], dest_channels[i], axis_number);
        if (result != LINK_SUCCESS) {
            printf("Failed to delete link %d\n", i);
            failed++;
            continue;
        }
        
        // Verify link count
        node = Core[source_node];
        channel_offset = get_channel_offset(node, source_ch);
        axis_offset = get_axis_offset(node, source_ch, axis_number);
        ushort current_link_count = *(ushort*)(node + channel_offset + axis_offset);
        
        if (current_link_count != initial_link_count + i) {
            printf("Link count mismatch after deletion %d: expected %d, got %d\n",
                   i, initial_link_count + i, current_link_count);
            failed++;
        }
    }
    
    // Verify final state
    node = Core[source_node];
    channel_offset = get_channel_offset(node, source_ch);
    axis_offset = get_axis_offset(node, source_ch, axis_number);
    ushort final_link_count = *(ushort*)(node + channel_offset + axis_offset);
    
    if (final_link_count != initial_link_count) {
        printf("Final link count mismatch: expected %d, got %d\n",
               initial_link_count, final_link_count);
        failed++;
    }
    
    printf("Link creation/deletion test completed: %d failures\n", failed);
    return failed;
} 