#include "link_tests.h"
#include "../Graph_structure/link.h"
#include "../Graph_structure/channel.h"
#include "../map.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int test_multiple_link_creation(uint source_node, ushort source_ch, ushort axis_number) {
    int failed = 0;
    srand(time(NULL));  // Initialize random seed
    
    printf("Testing multiple link creation...\n");
    printf("Source: node %d, Channel %d, Axis %d\n", source_node, source_ch, axis_number);
    
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
    printf("Source: node %d, Channel %d, Axis %d\n", source_node, source_ch, axis_number);
    
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
    uint dest_vertices[100];
    ushort dest_channels[100];
    
    // Create 100 sequential links
    printf("Creating 100 links...\n");
    for (int i = 0; i < 100; i++) {
        // Use sequential destination vertices and channels
        uint dest_node = (source_node + i + 1) % 256;  // Wrap around at 256
        ushort dest_ch = i % 5;  // Use channels 0-4
        
        // Store destinations for deletion
        dest_vertices[i] = dest_node;
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
        int result = delete_link(source_node, source_ch, dest_vertices[i], dest_channels[i], axis_number);
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

int test_multi_channel_links(uint node_index) {
    int failed = 0;
    printf("Testing link creation/deletion across multiple channels...\n");
    printf("Target node: %d\n", node_index);
    
    // Create second channel (channel 0 exists by default)
    // if (create_channel(node_index) != CHANNEL_SUCCESS) {
    //     printf("Failed to create channel 1\n");
    //     return -1;
    // }
    
    // Store initial state
    long node_position = get_node_position(node_index);
    uchar* node = Core[(unsigned int)node_position];
    uint ch0_offset = get_channel_offset(node, 0);
    uint ch1_offset = get_channel_offset(node, 1);
    
    // Ensure axis 0 exists in both channels
    if (!ensure_axis_exists(node_index, 0, 0) || 
        !ensure_axis_exists(node_index, 1, 0)) {
        printf("Failed to ensure axis exists in channels\n");
        return -1;
    }
    
    // Get initial link counts
    uint axis0_offset = get_axis_offset(node, 0, 0);
    uint axis1_offset = get_axis_offset(node, 1, 0);
    ushort initial_count_ch0 = *(ushort*)(node + ch0_offset + axis0_offset);
    ushort initial_count_ch1 = *(ushort*)(node + ch1_offset + axis1_offset);
    
    // Store link destinations for later deletion
    struct {
        uint node;
        ushort channel;
    } links_ch0[100], links_ch1[100];
    
    // Create 100 links in each channel
    printf("Creating 100 links in each channel...\n");
    for (int i = 0; i < 100; i++) {
        // Create link in channel 0
        uint dest_node = (node_index + i + 1) % 256;
        ushort dest_ch = i % 5;
        links_ch0[i].node = dest_node;
        links_ch0[i].channel = dest_ch;
        
        if (create_link(node_index, 0, dest_node, dest_ch, 0) != LINK_SUCCESS) {
            printf("Failed to create link %d in channel 0\n", i);
            failed++;
        }
        
        // Create link in channel 1 (different destinations)
        dest_node = (node_index + i + 2) % 256;
        dest_ch = (i + 1) % 5;
        links_ch1[i].node = dest_node;
        links_ch1[i].channel = dest_ch;
        
        if (create_link(node_index, 1, dest_node, dest_ch, 0) != LINK_SUCCESS) {
            printf("Failed to create link %d in channel 1\n", i);
            failed++;
        }
        
        // Verify link counts
        node_position = get_node_position(node_index);
        node = Core[(unsigned int)node_position];  // Reload node pointer as it might have changed
        ch0_offset = get_channel_offset(node, 0);
        ch1_offset = get_channel_offset(node, 1);
        axis0_offset = get_axis_offset(node, 0, 0);
        axis1_offset = get_axis_offset(node, 1, 0);
        
        ushort count_ch0 = *(ushort*)(node + ch0_offset + axis0_offset);
        ushort count_ch1 = *(ushort*)(node + ch1_offset + axis1_offset);
        
        if (count_ch0 != initial_count_ch0 + i + 1) {
            printf("Link count mismatch in ch0 after link %d: expected %d, got %d\n",
                   i, initial_count_ch0 + i + 1, count_ch0);
            failed++;
        }
        if (count_ch1 != initial_count_ch1 + i + 1) {
            printf("Link count mismatch in ch1 after link %d: expected %d, got %d\n",
                   i, initial_count_ch1 + i + 1, count_ch1);
            failed++;
        }
    }
    
    // Delete all links in reverse order
    printf("Deleting all links...\n");
    for (int i = 99; i >= 0; i--) {
        // Delete from channel 1 first
        if (delete_link(node_index, 1, links_ch1[i].node, links_ch1[i].channel, 0) != LINK_SUCCESS) {
            printf("Failed to delete link %d from channel 1\n", i);
            failed++;
        }
        
        // Then delete from channel 0
        if (delete_link(node_index, 0, links_ch0[i].node, links_ch0[i].channel, 0) != LINK_SUCCESS) {
            printf("Failed to delete link %d from channel 0\n", i);
            failed++;
        }
        
        // Verify link counts
        node_position = get_node_position(node_index);
        node = Core[node_position];
        ch0_offset = get_channel_offset(node, 0);
        ch1_offset = get_channel_offset(node, 1);
        axis0_offset = get_axis_offset(node, 0, 0);
        axis1_offset = get_axis_offset(node, 1, 0);
        
        ushort count_ch0 = *(ushort*)(node + ch0_offset + axis0_offset);
        ushort count_ch1 = *(ushort*)(node + ch1_offset + axis1_offset);
        
        if (count_ch0 != initial_count_ch0 + i) {
            printf("Link count mismatch in ch0 after deletion %d: expected %d, got %d\n",
                   i, initial_count_ch0 + i, count_ch0);
            failed++;
        }
        if (count_ch1 != initial_count_ch1 + i) {
            printf("Link count mismatch in ch1 after deletion %d: expected %d, got %d\n",
                   i, initial_count_ch1 + i, count_ch1);
            failed++;
        }
    }
    
    // Verify final state
    node_position = get_node_position(node_index);
    node = Core[(unsigned int)node_position];
    ch0_offset = get_channel_offset(node, 0);
    ch1_offset = get_channel_offset(node, 1);
    axis0_offset = get_axis_offset(node, 0, 0);
    axis1_offset = get_axis_offset(node, 1, 0);
    
    ushort final_count_ch0 = *(ushort*)(node + ch0_offset + axis0_offset);
    ushort final_count_ch1 = *(ushort*)(node + ch1_offset + axis1_offset);
    
    if (final_count_ch0 != initial_count_ch0) {
        printf("Final link count mismatch in ch0: expected %d, got %d\n",
               initial_count_ch0, final_count_ch0);
        failed++;
    }
    if (final_count_ch1 != initial_count_ch1) {
        printf("Final link count mismatch in ch1: expected %d, got %d\n",
               initial_count_ch1, final_count_ch1);
        failed++;
    }
    
    printf("Multi-channel link test completed: %d failures\n", failed);
    return failed;
} 