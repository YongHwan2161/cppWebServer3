#include "link.h"
#include "channel.h"
#include "free_space.h"
#include <stdio.h>
#include <string.h>

int create_link(int source_node, int source_ch, 
               int dest_node, int dest_ch, 
               int axis_number) {
    
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
        channel_offset = get_channel_offset(node, source_ch);
    }
    
    // Get axis offset - this points to the link count
    int axis_offset = get_axis_offset(node, source_ch, axis_number);
    if (axis_offset < 0) {
        printf("Error: Failed to get axis offset\n");
        return LINK_ERROR;
    }
    
    // Get current link count
    ushort* link_count = (ushort*)(node + axis_offset);
    ushort current_link_count = *link_count;
    
    // Calculate required space for new link
    uint current_node_size = 1 << (*(ushort*)node);
    uint link_data_offset = axis_offset + 2 + (current_link_count * 6);  // Skip link count and existing links
    ushort required_size = link_data_offset + sizeof(Link);
    
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
        
        // Recalculate offsets with new node pointer
        channel_offset = get_channel_offset(node, source_ch);
        axis_offset = get_axis_offset(node, source_ch, axis_number);
        link_count = (ushort*)(node + axis_offset);
        link_data_offset = axis_offset + 2 + (current_link_count * 6);
    }
    
    // Create link data
    Link link = {
        .node = dest_node,
        .channel = dest_ch
    };
    
    // Write link data after existing links
    memcpy(node + link_data_offset, &link, sizeof(Link));
    
    // Update link count
    (*link_count)++;
    
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