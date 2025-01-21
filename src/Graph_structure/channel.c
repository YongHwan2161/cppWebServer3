#include "channel.h"
#include "../free_space.h"
#include "node.h"
#include "../memory.h"
#include "../map.h"
#include <string.h>

ushort get_channel_count(uchar* node) {
    return *(ushort*)(node + 6);  // Skip size power (2) and actual size (4)
}

unsigned int get_channel_offset(uchar* node, ushort channel_index) {
    ushort channel_count = get_channel_count(node);
    if (channel_index >= channel_count) {
        printf("Error: Invalid channel index %d (max: %d)\n", 
               channel_index, channel_count - 1);
    }
    return *(uint*)(node + 8 + (channel_index * 4));  // 8: size_power(2) + actual_size(4) + channels(2)
}
unsigned int get_channel_end_offset(uchar* node, ushort channel_index) {
    ushort channel_count = get_channel_count(node);
    if (channel_index + 1 < channel_count) {
        uint next_channel_offset = get_channel_offset(node, channel_index + 1);
        return next_channel_offset;
    } else if (channel_index == channel_count - 1) {
        return *(uint*)(node + 2);
    }
    return 0; // This should never happen
}
int create_channel(uint node_index) {
    uint node_position = get_node_position(node_index);
    uchar* node = Core[node_position];
    // Get current actual size and calculate required size
    uint current_actual_size = *(uint*)(node + 2);
    uint required_size = current_actual_size + 6;  // channel entry(4) + axis count(2)
    
    if (check_and_resize_node(node, required_size, node_index) == FREE_SPACE_ERROR) {
        printf("Error: Failed to resize node\n");
        return CHANNEL_ERROR;
    }
    node_position = get_node_position(node_index);
    node = Core[node_position];
    ushort* channel_count = (ushort*)(node + 6);  // Skip size power(2) and actual size(4)
    printf("channel_count: %d\n", *channel_count);
    uint current_offset = 8 + ((uint)*channel_count * 4);  // Header + existing channel offsets
    uint channel_data_offset = current_actual_size + 4;  // New channel data goes at the end
    insert_uint(node, current_offset, channel_data_offset, current_actual_size - current_offset);

    //update the channel offset
    for (ushort i = 0; i < *channel_count; i++) {
        *(uint*)(node + 8 + (i * 4)) += 4;
    }
    
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
// Create multiple channels in a node
int create_multi_channels(uint node_index, int num_channels) {
    if (num_channels < 1) {
        printf("Error: Invalid number of channels to create\n");
        return CHANNEL_ERROR;
    }

    // Create channels one by one
    for (int i = 0; i < num_channels; i++) {
        if (create_channel(node_index) != CHANNEL_SUCCESS) {
            printf("Error: Failed to create channel %d\n", i + 1);
            return CHANNEL_ERROR;
        }
    }

    return CHANNEL_SUCCESS;
}
// Find empty channel or create new one
int recycle_or_create_channel(uint node_index) {
    uint node_position = get_node_position(node_index);
    if (!Core[node_position]) {
        printf("Error: node %u not loaded\n", node_index);
        return CHANNEL_ERROR;
    }

    uchar* node = Core[node_position];
    ushort channel_count = get_channel_count(node);

    // Check for empty channels
    for (ushort ch = 1; ch < channel_count; ch++) {
        uint channel_offset = get_channel_offset(node, ch);
        ushort axis_count = *(ushort*)(node + channel_offset);
        if (axis_count == 0) {
            return ch; // Found empty channel
        }
    }

    // No empty channels found, create new one
    if (create_channel(node_index) != CHANNEL_SUCCESS) {
        printf("Error: Failed to create new channel\n");
        return CHANNEL_ERROR;
    }

    return channel_count; // Return new channel index
}
// int clear_channel(uint node_index, ushort channel_index) {
//     uint node_position = get_node_position(node_index);
//     uchar* node = Core[node_position];
//     uint channel_offset = get_channel_offset(node, channel_index);
//     *(ushort*)(node + channel_offset) = 0;
//     return CHANNEL_SUCCESS;
// }
int clear_channel(uint node_index, ushort channel_index) {
    uint node_position = get_node_position(node_index);
    uchar* node = Core[node_position];
    uint channel_offset = get_channel_offset(node, channel_index);
    uint channel_end_offset = get_channel_end_offset(node, channel_index);
    uint move_dest = channel_offset + 2;
    uint actual_size = *(uint*)(node + 2);
    uint move_size = actual_size - channel_end_offset; // move_size is the size of the channel to be moved
    memmove(node + move_dest, node + channel_end_offset, move_size);

    // Update actual size - corrected calculation
    *(uint*)(node + 2) = actual_size - (channel_end_offset - move_dest);
    *(ushort*)(node + channel_offset) = 0; // Clear axis count

    // Update channel offsets
    ushort channel_count = get_channel_count(node);
    for (ushort i = channel_index + 1; i < channel_count; i++) {
        *(uint*)(node + 8 + (i * 4)) -= channel_end_offset - move_dest;
    }

    if (!save_node_to_file(node_index)) {
        printf("Error: Failed to update data.bin\n");
        return CHANNEL_ERROR;
    }

    return CHANNEL_SUCCESS;
}
int clear_channels(uint *node_index, ushort *channel_index, ushort count) {
    for (ushort i = 0; i < count; i++) {
        if (clear_channel(node_index[i], channel_index[i]) != CHANNEL_SUCCESS) {
            printf("Error: Failed to clear channel %d\n", i);
            return CHANNEL_ERROR;
        }
    }
    return CHANNEL_SUCCESS;
}
