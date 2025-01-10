#include "channel.h"
#include <string.h>

ushort get_channel_count(uchar* node) {
    return *(ushort*)(node + 6);  // Skip size power (2) and actual size (4)
}

uint get_channel_offset(uchar* node, int channel_index) {
    ushort channel_count = get_channel_count(node);
    if (channel_index >= channel_count) {
        printf("Error: Invalid channel index %d (max: %d)\n", 
               channel_index, channel_count - 1);
    }
    return *(uint*)(node + 8 + (channel_index * 4));  // 8: size_power(2) + actual_size(4) + channels(2)
}

ushort get_channel_size(uchar* node, int channel_index) {
    uint offset;
    
    // This will exit if channel_index is invalid
    offset = get_channel_offset(node, channel_index);
    
    // Channel size is stored in first 2 bytes of channel data
    return *(ushort*)(node + offset);
}

int create_channel(int node_index) {
    if (node_index >= 256 || !Core[node_index]) {
        return CHANNEL_ERROR;
    }
    
    uchar* node = Core[node_index];
    ushort* channel_count = (ushort*)(node + 2);
    
    // Calculate new channel offset
    int new_channel_offset = 4 + (*channel_count * 4);  // 4: size(2) + channels(2)
    
    // Initial channel size is 2 bytes (for axis count)
    int channel_size = 2;
    
    // Check if we need to resize the node
    ushort node_size_power = *(ushort*)node;
    uint current_node_size = 1 << node_size_power;
    uint required_size = new_channel_offset + 4 + channel_size;  // +4 for new channel offset
    
    // If required size is larger than current size, resize node
    if (required_size > current_node_size) {
        // Find next power of 2
        uint new_size = current_node_size;
        while (new_size < required_size) {
            new_size *= 2;
            node_size_power++;
        }
        
        // Allocate new space
        uchar* new_node = (uchar*)malloc(new_size);
        memcpy(new_node, node, current_node_size);
        free(node);
        node = new_node;
        Core[node_index] = new_node;
        
        // Update node size
        *(ushort*)node = node_size_power;
    }
    
    // Add new channel offset
    *(uint*)(node + new_channel_offset) = new_channel_offset + 4;
    
    // Initialize channel with 0 axes
    *(ushort*)(node + new_channel_offset + 4) = 0;
    
    // Update channel count
    (*channel_count)++;
    
    return CHANNEL_SUCCESS;
} 