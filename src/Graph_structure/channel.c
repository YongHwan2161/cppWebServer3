#include "channel.h"
#include "../free_space.h"
#include "vertex.h"
#include "../memory.h"
#include "../map.h"
#include <string.h>

ushort get_channel_count(uchar* vertex) {
    return *(ushort*)(vertex + 6);  // Skip size power (2) and actual size (4)
}

unsigned int get_channel_offset(uchar* vertex, ushort channel_index) {
    ushort channel_count = get_channel_count(vertex);
    if (channel_index >= channel_count) {
        printf("Error: Invalid channel index %d (max: %d)\n", 
               channel_index, channel_count - 1);
    }
    return *(uint*)(vertex + 8 + (channel_index * 4));  // 8: size_power(2) + actual_size(4) + channels(2)
}
unsigned int get_channel_end_offset(uchar* vertex, ushort channel_index) {
    ushort channel_count = get_channel_count(vertex);
    if (channel_index + 1 < channel_count) {
        uint next_channel_offset = get_channel_offset(vertex, channel_index + 1);
        return next_channel_offset;
    } else if (channel_index == channel_count - 1) {
        return *(uint*)(vertex + 2);
    }
    return 0; // This should never happen
}
int create_channel(uint vertex_index) {
    uint vertex_position = get_vertex_position(vertex_index);
    uchar* vertex = Core[vertex_position];
    // Get current actual size and calculate required size
    uint current_actual_size = *(uint*)(vertex + 2);
    uint required_size = current_actual_size + 6;  // channel entry(4) + axis count(2)
    
    if (check_and_resize_vertex(vertex, required_size, vertex_index) == FREE_SPACE_ERROR) {
        printf("Error: Failed to resize vertex\n");
        return CHANNEL_ERROR;
    }
    vertex_position = get_vertex_position(vertex_index);
    vertex = Core[vertex_position];
    ushort* channel_count = (ushort*)(vertex + 6);  // Skip size power(2) and actual size(4)
    printf("channel_count: %d\n", *channel_count);
    uint current_offset = 8 + ((uint)*channel_count * 4);  // Header + existing channel offsets
    uint channel_data_offset = current_actual_size + 4;  // New channel data goes at the end
    insert_uint(vertex, current_offset, channel_data_offset, current_actual_size - current_offset);

    //update the channel offset
    for (ushort i = 0; i < *channel_count; i++) {
        *(uint*)(vertex + 8 + (i * 4)) += 4;
    }
    
    // Initialize axis count to 0 at the end
    *(ushort*)(vertex + channel_data_offset) = 0;
    
    // Update actual size
    *(uint*)(vertex + 2) = required_size;
    
    // Increment channel count
    (*channel_count)++;

    if (!save_vertex_to_file(vertex_index)) {
        printf("Error: Failed to update data.bin\n");
        return CHANNEL_ERROR;
    }
    return CHANNEL_SUCCESS;
}
// Create multiple channels in a vertex
int create_multi_channels(uint vertex_index, int num_channels) {
    if (num_channels < 1) {
        printf("Error: Invalid number of channels to create\n");
        return CHANNEL_ERROR;
    }

    // Create channels one by one
    for (int i = 0; i < num_channels; i++) {
        if (create_channel(vertex_index) != CHANNEL_SUCCESS) {
            printf("Error: Failed to create channel %d\n", i + 1);
            return CHANNEL_ERROR;
        }
    }

    return CHANNEL_SUCCESS;
}
// Find empty channel or create new one
int recycle_or_create_channel(uint vertex_index) {
    uint vertex_position = get_vertex_position(vertex_index);
    if (!Core[vertex_position]) {
        printf("Error: Vertex %u not loaded\n", vertex_index);
        return CHANNEL_ERROR;
    }

    uchar* vertex = Core[vertex_position];
    ushort channel_count = get_channel_count(vertex);

    // Check for empty channels
    for (ushort ch = 1; ch < channel_count; ch++) {
        uint channel_offset = get_channel_offset(vertex, ch);
        ushort axis_count = *(ushort*)(vertex + channel_offset);
        if (axis_count == 0) {
            return ch; // Found empty channel
        }
    }

    // No empty channels found, create new one
    if (create_channel(vertex_index) != CHANNEL_SUCCESS) {
        printf("Error: Failed to create new channel\n");
        return CHANNEL_ERROR;
    }

    return channel_count; // Return new channel index
}
// int clear_channel(uint vertex_index, ushort channel_index) {
//     uint vertex_position = get_vertex_position(vertex_index);
//     uchar* vertex = Core[vertex_position];
//     uint channel_offset = get_channel_offset(vertex, channel_index);
//     *(ushort*)(vertex + channel_offset) = 0;
//     return CHANNEL_SUCCESS;
// }
int clear_channel(uint vertex_index, ushort channel_index) {
    uint vertex_position = get_vertex_position(vertex_index);
    uchar* vertex = Core[vertex_position];
    uint channel_offset = get_channel_offset(vertex, channel_index);
    uint channel_end_offset = get_channel_end_offset(vertex, channel_index);
    uint move_dest = channel_offset + 2;
    uint actual_size = *(uint*)(vertex + 2);
    uint move_size = actual_size - channel_end_offset; // move_size is the size of the channel to be moved
    memmove(vertex + move_dest, vertex + channel_end_offset, move_size);

    // Update actual size - corrected calculation
    *(uint*)(vertex + 2) = actual_size - (channel_end_offset - move_dest);
    *(ushort*)(vertex + channel_offset) = 0; // Clear axis count

    // Update channel offsets
    ushort channel_count = get_channel_count(vertex);
    for (ushort i = channel_index + 1; i < channel_count; i++) {
        *(uint*)(vertex + 8 + (i * 4)) -= channel_end_offset - move_dest;
    }

    if (!save_vertex_to_file(vertex_index)) {
        printf("Error: Failed to update data.bin\n");
        return CHANNEL_ERROR;
    }

    return CHANNEL_SUCCESS;
}
int clear_channels(uint *vertex_index, ushort *channel_index, ushort count) {
    for (ushort i = 0; i < count; i++) {
        if (clear_channel(vertex_index[i], channel_index[i]) != CHANNEL_SUCCESS) {
            printf("Error: Failed to clear channel %d\n", i);
            return CHANNEL_ERROR;
        }
    }
    return CHANNEL_SUCCESS;
}
