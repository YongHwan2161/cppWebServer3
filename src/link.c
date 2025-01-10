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
    uint axis_offset = get_axis_offset(node, source_ch, axis_number);
    
    // if (axis_offset == -1) return LINK_ERROR;
    
    // Get current link count and calculate new size
    ushort* link_count = (ushort*)(node + channel_offset + axis_offset);
    uint current_actual_size = *(uint*)(node + 2);
    uint required_size = current_actual_size + 6;  // Add 6 bytes for new link
    
    // Check if we need to resize
    ushort node_size_power = *(ushort*)node;
    uint current_node_size = 1 << node_size_power;
    
    if (required_size > current_node_size) {
        uint new_size;
        node = resize_node_space(node, required_size, source_node, &new_size);
        if (!node) {
            printf("Error: Failed to resize node\n");
            return LINK_ERROR;
        }
        Core[source_node] = node;
        // Recalculate offsets as node pointer might have changed
        channel_offset = get_channel_offset(node, source_ch);
        axis_offset = get_axis_offset(node, source_ch, axis_number);
        link_count = (ushort*)(node + channel_offset + axis_offset);
    }
    
    // Add new link at the end
    uint link_offset = channel_offset + axis_offset + 2 + (*link_count * 6);
    *(uint*)(node + link_offset) = dest_node;
    *(ushort*)(node + link_offset + 4) = dest_ch;
    
    // Update link count and actual size
    (*link_count)++;
    *(uint*)(node + 2) = required_size;
    
    // Save changes to data.bin
    FILE* data_file = fopen(DATA_FILE, "r+b");
    if (data_file) {
        fseek(data_file, CoreMap[source_node].file_offset, SEEK_SET);
        fwrite(node, 1, current_node_size, data_file);
        fclose(data_file);
        return LINK_SUCCESS;
    }
    
    printf("Error: Failed to update data.bin\n");
    return LINK_ERROR;
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