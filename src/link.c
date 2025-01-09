#include "link.h"
#include "channel.h"
#include "free_space.h"
#include <stdio.h>
#include <string.h>

int create_link(uint source_node, ushort source_ch, 
               uint dest_node, ushort dest_ch, 
               ushort axis_number) {
    
    // Validate source node
    if (!Core[source_node]) {
        printf("Error: Invalid source node index\n");
        return LINK_ERROR;
    }
    
    uchar* node = Core[source_node];
    uint channel_offset = get_channel_offset(node, source_ch);
    
    // Check if axis exists, create if it doesn't
    if (!has_axis(node, channel_offset, axis_number)) {
        int result = create_axis(source_node, source_ch, axis_number);
        if (result != AXIS_SUCCESS) {
            printf("Error: Failed to create required axis\n");
            return LINK_ERROR;
        }
        
        // Reload node pointer as it might have changed after axis creation
        node = Core[source_node];
    }
    
    // Get axis offset - this points to the link count
    uint axis_offset = get_axis_offset(node, source_ch, axis_number);
    
    // Get current link count
    // ushort* link_count = (ushort*)(node + channel_offset + axis_offset);
    ushort current_link_count = *(ushort*)(node + channel_offset + axis_offset);
    
    // Calculate required space for new link
    uint current_node_size = 1 << (*(ushort*)node);
    uint last_axis_offset = get_last_axis_offset(node, source_ch);  // Get offset of last axis
    uint last_link_offset = channel_offset + last_axis_offset + 2 + (current_link_count * 6);  // Current end of link data
    uint required_size = last_link_offset + 6;  // Add space for one new link (6 bytes)
    
    // Check if we need more space
    if (required_size > current_node_size) {
        uint new_size;
        node = resize_node_space(node, required_size, source_node, &new_size);
        if (!node) {
            printf("Error: Failed to resize node\n");
            return LINK_ERROR;
        }
        
        // Update Core pointer
        Core[source_node] = node;
        // link_count = (ushort*)(node + channel_offset + axis_offset);
    }
    
    // Create link data
    Link link = {
        .node = dest_node,
        .channel = dest_ch
    };
    
    // Calculate insert position for new link
    uint link_insert_offset = channel_offset + axis_offset + 2 + (current_link_count * 6);
    
    // Check if this is not the last channel and not the last axis
    ushort channel_count = *(ushort*)(node + 2);  // Get channel count
    bool is_last_channel = (source_ch == channel_count - 1);
    bool is_last_axis = (axis_offset == last_axis_offset);
    
    // Move data forward if this is not the last position
    if (!is_last_channel || !is_last_axis) {
        // Calculate amount of data to move
        uint move_start = link_insert_offset;
        uint move_size = required_size - move_start;  // Move all remaining data
        
        // Move existing data forward by 6 bytes
        memmove(node + move_start + 6,
                node + move_start,
                move_size);
        
        // Update offsets in current channel
        uint axis_data_offset = channel_offset + 2;
        ushort axis_count = *(ushort*)(node + channel_offset);
        
        for (int i = 0; i < axis_count; i++) {
            uint current_axis_offset = *(uint*)(node + axis_data_offset + (i * 6) + 2);
            if (current_axis_offset > axis_offset) {
                *(uint*)(node + axis_data_offset + (i * 6) + 2) += 6;
            }
        }
        
        // Update only channel offsets for subsequent channels
        if (!is_last_channel) {
            for (int ch = source_ch + 1; ch < channel_count; ch++) {
                uint* channel_offset_ptr = (uint*)(node + 4 + (ch * 4));
                *channel_offset_ptr += 6;
            }
        }
    }
    
    // Write link data at insert position
    memcpy(node + link_insert_offset, &link, sizeof(Link));
    
    // Update link count - recalculate pointer after possible node resize
    // ushort* link_count = (ushort*)(node + channel_offset + axis_offset);
    (*(ushort*)(node + channel_offset + axis_offset))++;
    
    // Save changes to data.bin
    FILE* data_file = fopen(DATA_FILE, "r+b");
    if (data_file) {
        fseek(data_file, CoreMap[source_node].file_offset, SEEK_SET);
        fwrite(node, 1, 1 << (*(ushort*)node), data_file);
        fclose(data_file);
    }
    
    // Save updated free space information
    save_free_space();
    
    printf("Created link from node %d channel %d to node %d channel %d using axis %d\n",
           source_node, source_ch, dest_node, dest_ch, axis_number);
    
    return LINK_SUCCESS;
}

int delete_link(uint source_node, ushort source_ch, 
               uint dest_node, ushort dest_ch, 
               ushort axis_number) {
    
    // Validate source node
    if (!Core[source_node]) {
        printf("Error: Invalid source node index\n");
        return LINK_ERROR;
    }
    
    uchar* node = Core[source_node];
    uint channel_offset = get_channel_offset(node, source_ch);
    
    // Check if axis exists
    if (!has_axis(node, channel_offset, axis_number)) {
        printf("Error: Axis %d does not exist in node %d, channel %d\n",
               axis_number, source_node, source_ch);
        return LINK_ERROR;
    }
    
    // Get axis offset and link count
    uint axis_offset = get_axis_offset(node, source_ch, axis_number);
    ushort* link_count = (ushort*)(node + axis_offset);
    uint link_data_offset = axis_offset + 2;  // Skip link count
    
    // Search for the link
    bool found = false;
    int link_position = -1;
    
    for (int i = 0; i < *link_count; i++) {
        Link* current_link = (Link*)(node + link_data_offset + (i * sizeof(Link)));
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
    
    // Shift remaining links to fill the gap
    if (link_position < *link_count - 1) {
        memmove(node + link_data_offset + (link_position * sizeof(Link)),
                node + link_data_offset + ((link_position + 1) * sizeof(Link)),
                (*link_count - link_position - 1) * sizeof(Link));
    }
    
    // Update link count
    (*link_count)--;
    
    // Save changes to data.bin
    FILE* data_file = fopen(DATA_FILE, "r+b");
    if (data_file) {
        fseek(data_file, CoreMap[source_node].file_offset, SEEK_SET);
        fwrite(node, 1, 1 << (*(ushort*)node), data_file);
        fclose(data_file);
    } else {
        printf("Error: Failed to update data.bin\n");
        return LINK_ERROR;
    }
    
    printf("Successfully deleted link from node %d channel %d to node %d channel %d using axis %d\n",
           source_node, source_ch, dest_node, dest_ch, axis_number);
    
    return LINK_SUCCESS;
} 