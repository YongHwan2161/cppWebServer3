#include "channel.h"
#include "free_space.h"
#include "node.h"
#include <string.h>

ushort get_channel_count(uchar* node) {
    return *(ushort*)(node + 6);  // Skip size power (2) and actual size (4)
}

uint get_channel_offset(uchar* node, ushort channel_index) {
    ushort channel_count = get_channel_count(node);
    if (channel_index >= channel_count) {
        printf("Error: Invalid channel index %d (max: %d)\n", 
               channel_index, channel_count - 1);
    }
    return *(uint*)(node + 8 + (channel_index * 4));  // 8: size_power(2) + actual_size(4) + channels(2)
}

ushort get_channel_size(uchar* node, ushort channel_index) {
    uint offset = get_channel_offset(node, channel_index);
    return *(ushort*)(node + offset);
}

int create_channel(uint node_index) {
    uchar* node = Core[node_index];
    ushort* channel_count = (ushort*)(node + 6);  // Skip size power(2) and actual size(4)
    printf("channel_count: %d\n", *channel_count);
    // Get current actual size and calculate required size
    uint current_actual_size = *(uint*)(node + 2);
    uint required_size = current_actual_size + 6;  // channel entry(4) + axis count(2)
    
    // Check if resize needed
    ushort node_size_power = *(ushort*)node;
    uint current_node_size = 1 << node_size_power;
    
    if (required_size > current_node_size) {
        uint new_size;
        node = resize_node_space(node, required_size, node_index, &new_size);
        if (!node) {
            printf("Error: Failed to resize node\n");
            return CHANNEL_ERROR;
        }
        Core[node_index] = node;
        channel_count = (ushort*)(node + 6);
    }
    // Calculate offsets
    uint current_offset = 8 + ((uint)*channel_count * 4);  // Header + existing channel offsets
    uint channel_data_offset = current_actual_size + 4;  // New channel data goes at the end
    // Move existing data 4 bytes forward to make space for new channel entry
        memmove(node + current_offset + 4,          // destination (4 bytes forward)
                node + current_offset,               // source
                current_actual_size - current_offset // size of data to move
        );
    
    //update the channel offset
    for (ushort i = 0; i < *channel_count; i++) {
        *(uint*)(node + 8 + (i * 4)) += 4;
    }
    // Add new channel offset
    *(uint*)(node + current_offset) = channel_data_offset;
    
    // Initialize axis count to 0 at the end
    *(ushort*)(node + channel_data_offset) = 0;
    
    // Update actual size
    *(uint*)(node + 2) = required_size;
    
    // Increment channel count
    (*channel_count)++;

    if (!save_node_to_file(node_index)) {
        printf("Error: Failed to update data.bin\n");
        return CHANNEL_ERROR;
    }
    return CHANNEL_SUCCESS;
}