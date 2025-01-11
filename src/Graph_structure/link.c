#include "link.h"
#include "channel.h"
#include "../free_space.h"
#include "node.h"
#include <stdio.h>
#include <string.h>

int create_link(uint source_node, ushort source_ch, 
                uint dest_node, ushort dest_ch, 
                ushort axis_number) {
    
    uchar* node = Core[source_node];
    uint channel_offset = get_channel_offset(node, source_ch);
    // printf("Channel offset: %d\n", channel_offset);
    // printf("Axis number: %d\n", axis_number);
    // Check if axis exists, create if it doesn't

    uint axis_offset = get_axis_offset(node, source_ch, axis_number);
    // printf("Axis offset: %d\n", axis_offset);
    // Get current link count and calculate new size
    uint current_actual_size = *(uint*)(node + 2);
    uint required_size = current_actual_size + 6;  // Add 6 bytes for new link
    
    // Check if we need to resize
    ushort node_size_power = *(ushort*)node;
    uint current_node_size = 1 << node_size_power;
    // printf("Current node size: %d\n", current_node_size);
    if (required_size > current_node_size) {
        uint new_size;
        node = resize_node_space(node, required_size, source_node, &new_size);
        if (!node) {
            printf("Error: Failed to resize node\n");
            return LINK_ERROR;
        }
        Core[source_node] = node;
    }

    ushort channel_count = get_channel_count(node);  // Get channel count
    ushort* current_link_count = (ushort*)(node + channel_offset + axis_offset);
    // Calculate insert position for new link
    uint link_insert_offset = channel_offset + axis_offset + 2 + (*current_link_count * 6);
    // printf("Link insert offset: %d\n", link_insert_offset);
    if (link_insert_offset < current_actual_size) {
    // Move data forward if this is not the last position

        // Calculate amount of data to move
        uint move_start = link_insert_offset;
        uint move_size = current_actual_size - move_start;  // Move all remaining data
        // Move existing data forward by 6 bytes
        memmove(node + move_start + 6,
                node + move_start,
                move_size);
        // Update offsets in current channel
        uint axis_data_offset = channel_offset + 2;
        ushort axis_count = get_axis_count(node, source_ch);
        // printf("Axis count: %d\n", axis_count);
        for (ushort i = 0; i < axis_count; i++) {
            uint current_axis_offset = *(uint*)(node + axis_data_offset + (i * 6) + 2);
            if (current_axis_offset > axis_offset) {
                *(uint*)(node + axis_data_offset + (i * 6) + 2) += 6;
            }
        }
        // Update only channel offsets for subsequent channels
        if (source_ch < channel_count - 1) {
            for (ushort ch = source_ch + 1; ch < channel_count; ch++) {
                uint* channel_offset_ptr = (uint*)(node + 4 + (ch * 4));
                *channel_offset_ptr += 6;
            }
        }
    }
    // Update link count and actual size
    (*current_link_count)++;
    *(uint*)(node + 2) = required_size;
    
    // Write link data
    *(uint*)(node + link_insert_offset) = dest_node;
    *(ushort*)(node + link_insert_offset + 4) = dest_ch;
    
    if (!save_node_to_file(source_node)) {
        printf("Error: Failed to update data.bin\n");
        return LINK_ERROR;
    }
    return LINK_SUCCESS;
}

int delete_link(uint source_node, ushort source_ch, 
               uint dest_node, ushort dest_ch, 
               ushort axis_number) {
    
    uchar* node = Core[source_node];
    uint channel_offset = get_channel_offset(node, source_ch);
    
    // Get axis offset and link count
    uint axis_offset = get_axis_offset(node, source_ch, axis_number);
    ushort* current_link_count = (ushort*)(node + channel_offset + axis_offset);
    uint link_data_offset = channel_offset + axis_offset + 2;  // Skip link count
    printf("Link data offset: %d\n", link_data_offset);
    // Search for the link
    bool found = false;
    int link_position = -1;
    
    for (int i = 0; i < *current_link_count; i++) {
        Link* current_link = (Link*)(node + link_data_offset + (i * 6));
        if (current_link->node == dest_node && current_link->channel == dest_ch) {
            found = true;
            link_position = i;
            break;
        }
    }
    
    if (!found) {
        printf("Error: Link not found\n");
        return LINK_ERROR;
    }
    
    uint target_link_offset = link_data_offset + (link_position * 6);
    printf("Target link offset: %d\n", target_link_offset);
    uint actual_size = *(uint*)(node + 2);
    printf("Actual size: %d\n", actual_size);
    // Shift remaining links to fill the gap
    if (target_link_offset < actual_size - 6) {
        memmove(node + target_link_offset,
                node + target_link_offset + 6,
                actual_size - target_link_offset - 6);
    }
    
    // Update link count and actual size
    (*current_link_count)--;
    *(uint*)(node + 2) = actual_size - 6;
    
    if (!save_node_to_file(source_node)) {
        printf("Error: Failed to update data.bin\n");
        return LINK_ERROR;
    }
    return LINK_SUCCESS;
} 