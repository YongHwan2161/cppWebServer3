#include "channel.h"
#include "../free_space.h"
#include "vertex.h"
#include "../memory.h"
#include <string.h>

ushort get_channel_count(uchar* vertex) {
    return *(ushort*)(vertex + 6);  // Skip size power (2) and actual size (4)
}

uint get_channel_offset(uchar* vertex, ushort channel_index) {
    ushort channel_count = get_channel_count(vertex);
    if (channel_index >= channel_count) {
        printf("Error: Invalid channel index %d (max: %d)\n", 
               channel_index, channel_count - 1);
    }
    return *(uint*)(vertex + 8 + (channel_index * 4));  // 8: size_power(2) + actual_size(4) + channels(2)
}
uint get_channel_end_offset(uchar* vertex, ushort channel_index) {
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
int clear_channel(uint vertex_index, ushort channel_index) {
    uint vertex_position = get_vertex_position(vertex_index);
    uchar* vertex = Core[vertex_position];
    uint channel_offset = get_channel_offset(vertex, channel_index);
    uint channel_end_offset = get_channel_end_offset(vertex, channel_index);
    uint move_dest = channel_end_offset + 2;
    uint actual_size = *(uint*)(vertex + 2);
    uint move_size = actual_size - channel_end_offset;
    memmove(vertex + move_dest, vertex + channel_end_offset, move_size);

    // Update actual size
    *(uint*)(vertex + 2) = actual_size - (channel_end_offset - channel_offset);
    *(ushort*)(vertex + channel_offset) = 0;

    // Update channel offsets
    ushort channel_count = get_channel_count(vertex);
    uint delete_size = channel_end_offset - channel_offset - 2;
    printf("delete_size: %d\n", delete_size);
    printf("channel_end_offset: %d\n", channel_end_offset);
    printf("channel_offset: %d\n", channel_offset);
    for (ushort i = channel_index + 1; i < channel_count; i++) {
        *(uint*)(vertex + 8 + (i * 4)) -= delete_size;
    }

    if (!save_vertex_to_file(vertex_index)) {
        printf("Error: Failed to update data.bin\n");
        return CHANNEL_ERROR;
    }

    return CHANNEL_SUCCESS;
}