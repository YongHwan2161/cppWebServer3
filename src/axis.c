#include "axis.h"
#include "channel.h"
#include "free_space.h"
#include "link.h"
#include <string.h>
#include <stdbool.h>

int get_axis_count(uchar* node, ushort channel_index) {
    int offset = get_channel_offset(node, channel_index);
    if (offset < 0) return -1;
    
    return *(ushort*)(node + offset);  // First 2 bytes contain axis count
}

int get_axis_offset(uchar* node, ushort channel_index, ushort axis_number) {
    int channel_offset = get_channel_offset(node, channel_index);
    if (channel_offset < 0) return -1;
    
    ushort axis_count = *(ushort*)(node + channel_offset);
    int axis_data_offset = channel_offset + 2;  // Skip axis count
    
    // Search for the axis
    for (int i = 0; i < axis_count; i++) {
        ushort current_axis = *(ushort*)(node + axis_data_offset + (i * 6));
        if (current_axis == axis_number) {
            return *(uint*)(node + axis_data_offset + (i * 6) + 2);
        }
    }
    
    return -1;  // Axis not found
}

bool has_axis(uchar* node, uint channel_offset, ushort axis_number) {
    ushort axis_count = *(ushort*)(node + channel_offset);
    int axis_data_offset = channel_offset + 2;  // Skip axis count
    
    // Search for the axis
    for (int i = 0; i < axis_count; i++) {
        ushort current_axis = *(ushort*)(node + axis_data_offset + (i * 6));
        if (current_axis == axis_number) {
            return true;
        }
    }
    
    return false;
}

int create_axis(uint node_index, ushort channel_index, ushort axis_number) {
    // Validate node
    if (!Core[node_index]) {
        printf("Error: Invalid node index\n");
        return AXIS_ERROR;
    }
    
    uchar* node = Core[node_index];
    uint channel_offset = get_channel_offset(node, channel_index);
    
    // Get current axis count
    ushort* axis_count = (ushort*)(node + channel_offset);
    ushort current_count = *axis_count;
    
    // Check if axis already exists
    if (has_axis(node, channel_offset, axis_number)) {
        printf("Error: Axis %d already exists\n", axis_number);
        return AXIS_ERROR;
    }
    
    // Calculate required space
    uint axis_table_size = (current_count + 1) * 6;  // Including new axis entry
    uint last_axis_offset = 0;
    uint last_axis_data_size = 0;
    
    if (current_count > 0) {
        // Get last axis's offset and data size
        uint* last_offset_ptr = (uint*)(node + channel_offset + 2 + ((current_count - 1) * 6) + 2);
        last_axis_offset = *last_offset_ptr;
        ushort* last_link_count = (ushort*)(node + channel_offset + last_axis_offset);
        last_axis_data_size = 2 + (*last_link_count * sizeof(Link));  // 2 for link count
    }
    
    // Calculate total required size
    uint required_size = channel_offset + axis_table_size;  // Channel header + axis table
    if (current_count > 0) {
        required_size = channel_offset + last_axis_offset + last_axis_data_size + 6 + 2;
    } else {
        required_size += 2;  // Just need space for new axis's link count
    }
    
    // Check if we need more space
    uint current_node_size = 1 << (*(ushort*)node);
    if (required_size > current_node_size) {
        uint new_size;
        node = resize_node_space(node, required_size, node_index, &new_size);
        if (!node) {
            printf("Error: Failed to resize node\n");
            return AXIS_ERROR;
        }
        Core[node_index] = node;
        channel_offset = get_channel_offset(node, channel_index);
        axis_count = (ushort*)(node + channel_offset);
    }
    
    // Move existing axis data forward
    if (current_count > 0) {
        uint data_start = channel_offset + 2 + (current_count * 6);
        uint data_size = last_axis_offset + last_axis_data_size - (current_count * 6);
        
        // Move data forward by 6 bytes
        memmove(node + data_start + 6, 
                node + data_start, 
                data_size);
        
        // Update all existing axis offsets
        for (int i = 0; i < current_count; i++) {
            uint* offset_ptr = (uint*)(node + channel_offset + 2 + (i * 6) + 2);
            *offset_ptr += 6;
        }
    }
    
    // Add new axis entry
    uint new_axis_offset = (current_count > 0) ? 
                          last_axis_offset + last_axis_data_size + 6 : 
                          2 + axis_table_size;
    
    ushort* new_axis_number = (ushort*)(node + channel_offset + 2 + (current_count * 6));
    uint* new_axis_offset_ptr = (uint*)(node + channel_offset + 2 + (current_count * 6) + 2);
    
    *new_axis_number = axis_number;
    *new_axis_offset_ptr = new_axis_offset;
    
    // Initialize new axis data (link count = 0)
    ushort* new_link_count = (ushort*)(node + channel_offset + new_axis_offset);
    *new_link_count = 0;
    
    // Update axis count
    (*axis_count)++;
    
    // Save changes to data.bin
    FILE* data_file = fopen(DATA_FILE, "r+b");
    if (data_file) {
        fseek(data_file, CoreMap[node_index].file_offset, SEEK_SET);
        fwrite(node, 1, 1 << (*(ushort*)node), data_file);
        fclose(data_file);
    } else {
        printf("Error: Failed to update data.bin\n");
        return AXIS_ERROR;
    }
    
    printf("Created axis %d in node %d, channel %d\n",
           axis_number, node_index, channel_index);
    
    return AXIS_SUCCESS;
}

int delete_axis(uint node_index, ushort channel_index, ushort axis_number) {
    if (!Core[node_index]) {
        printf("Error: Invalid node index\n");
        return AXIS_ERROR;
    }
    
    uchar* node = Core[node_index];
    uint channel_offset = get_channel_offset(node, channel_index);
    
    // Check if axis exists
    if (!has_axis(node, channel_offset, axis_number)) {
        printf("Error: Axis %d does not exist in node %d, channel %d\n",
               axis_number, node_index, channel_index);
        return AXIS_ERROR;
    }
    
    // Get current axis count
    ushort* axis_count = (ushort*)(node + channel_offset);
    ushort current_axis_count = *axis_count;
    
    // Find the axis position
    int axis_data_offset = channel_offset + 2;  // Skip axis count
    int axis_position = -1;
    
    for (int i = 0; i < current_axis_count; i++) {
        if (*(ushort*)(node + axis_data_offset + (i * 6)) == axis_number) {
            axis_position = i;
            break;
        }
    }
    
    // Shift remaining axes to fill the gap
    if (axis_position < current_axis_count - 1) {
        memmove(node + axis_data_offset + (axis_position * 6),
                node + axis_data_offset + ((axis_position + 1) * 6),
                (current_axis_count - axis_position - 1) * 6);
    }
    
    // Update axis count
    (*axis_count)--;
    
    // Save changes to data.bin
    FILE* data_file = fopen(DATA_FILE, "r+b");
    if (data_file) {
        fseek(data_file, CoreMap[node_index].file_offset, SEEK_SET);
        fwrite(node, 1, 1 << (*(ushort*)node), data_file);
        fclose(data_file);
    }
    
    printf("Deleted axis %d from node %d, channel %d\n",
           axis_number, node_index, channel_index);
    return AXIS_SUCCESS;
} 