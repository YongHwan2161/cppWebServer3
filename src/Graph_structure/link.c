#include "link.h"
#include "channel.h"
#include "../free_space.h"
#include "node.h"
#include "../memory.h"
#include <stdio.h>
#include <string.h>

int create_link(uint source_node, ushort source_ch, 
                uint dest_node, ushort dest_ch, 
                ushort axis_number) {
    
    uchar* node = Core[source_node];
    uint channel_offset = get_channel_offset(node, source_ch);
    printf("channel_offset: %d\n", channel_offset);
    uint axis_offset = get_axis_offset(node, source_ch, axis_number);

    uint current_actual_size = *(uint*)(node + 2);
    uint required_size = current_actual_size + 6;  // Add 6 bytes for new link
    // Check and resize if needed using the new function
    int resize_result = check_and_resize_node(node, required_size, source_node);
    if (resize_result == FREE_SPACE_ERROR) {
        printf("Error: Failed to resize node\n");
        return LINK_ERROR;
    }
    node = Core[source_node];
    
    ushort channel_count = get_channel_count(node);  // Get channel count
    ushort* current_link_count = (ushort*)(node + channel_offset + axis_offset);
    uint link_insert_offset = channel_offset + axis_offset + 2 + (*current_link_count * 6);
    uint move_size = current_actual_size - link_insert_offset;  // Move all remaining data
    insert_link(node, link_insert_offset, dest_node, dest_ch, move_size);
    // Update offsets in current channel
    uint axis_entry_offset = channel_offset + 2;
    ushort axis_count = get_axis_count(node, source_ch);
    for (ushort i = 0; i < axis_count; i++)
    {
        uint current_axis_offset = *(uint *)(node + axis_entry_offset + (i * 6) + 2);
        if (current_axis_offset > axis_offset)
        {
            *(uint *)(node + axis_entry_offset + (i * 6) + 2) += 6;
        }
    }
    // Update only channel offsets for subsequent channels
    if (source_ch < channel_count - 1)
    {
        for (ushort ch = source_ch + 1; ch < channel_count; ch++)
        {
            *(uint *)(node + 8 + (ch * 4)) += 6;
        }
    }

    // Update link count and actual size
    (*current_link_count)++;
    *(uint*)(node + 2) = required_size;
    
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
    ushort axis_index = get_axis_index(node, source_ch, axis_number);
    // Get axis offset and link count
    uint axis_offset = get_axis_offset_by_index(node, source_ch, axis_index);
    ushort* current_link_count = (ushort*)(node + channel_offset + axis_offset);
    uint link_data_offset = channel_offset + axis_offset + 2;  // Skip link count
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
    uint actual_size = *(uint*)(node + 2);
    // Shift remaining links to fill the gap
    if (target_link_offset < actual_size - 6) {
        memmove(node + target_link_offset,
                node + target_link_offset + 6,
                actual_size - target_link_offset - 6);
    }
    uint current_axis_count = get_axis_count(node, source_ch);
    uint axis_entry_offset = channel_offset + 2;
    for (ushort i = axis_index; i < current_axis_count; i++) {
        uint current_axis_offset = *(uint *)(node + axis_entry_offset + (i * 6) + 2);
        if (current_axis_offset > axis_offset) {
            *(uint *)(node + axis_entry_offset + (i * 6) + 2) -= 6;
        }
    }
    ushort channel_count = get_channel_count(node);
    for (ushort ch = source_ch + 1; ch < channel_count; ch++) {
        *(uint *)(node + 8 + (ch * 4)) -= 6;
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