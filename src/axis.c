#include "axis.h"
#include "channel.h"
#include "free_space.h"
#include <string.h>
#include <stdbool.h>

int get_axis_count(uchar* node, int channel_index) {
    int offset = get_channel_offset(node, channel_index);
    if (offset < 0) return -1;
    
    return *(ushort*)(node + offset);  // First 2 bytes contain axis count
}

int get_axis_offset(uchar* node, int channel_index, int axis_number) {
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

bool has_axis(uchar* node, uint channel_offset, int axis_number) {
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

int create_axis(int node_index, int channel_index, int axis_number) {
    if (!Core[node_index]) {
        printf("Error: Invalid node index\n");
        return AXIS_ERROR;
    }
    
    uchar* node = Core[node_index];
    uint channel_offset = get_channel_offset(node, channel_index);
    
    // Get current axis count
    ushort* axis_count = (ushort*)(node + channel_offset);
    ushort current_axis_count = *axis_count;
    
    // Check if axis already exists
    if (has_axis(node, channel_offset, axis_number)) {
        printf("Error: Axis %d already exists in node %d, channel %d\n",
               axis_number, node_index, channel_index);
        return AXIS_ERROR;
    }
    
    // Calculate required space
    uint axis_data_offset = channel_offset + 2 + (current_axis_count * 6);  // Skip axis count and existing axes
    uint link_data_offset = axis_data_offset + 6;  // Space for new axis entry
    ushort required_size = link_data_offset + 2;  // Add space for link count
    
    // Check if we need more space
    uint current_node_size = 1 << (*(ushort*)node);
    if (required_size > current_node_size) {
        uint new_size;
        node = resize_node_space(node, required_size, node_index, &new_size);
        if (!node) {
            printf("Error: Failed to resize node\n");
            return AXIS_ERROR;
        }
        
        // Update Core pointer
        Core[node_index] = node;
        
        // Recalculate offsets with new node pointer
        channel_offset = get_channel_offset(node, channel_index);
        axis_data_offset = channel_offset + 2 + (current_axis_count * 6);
        link_data_offset = axis_data_offset + 6;
        axis_count = (ushort*)(node + channel_offset);
    }
    
    // Write axis number and offset
    *(ushort*)(node + axis_data_offset) = (ushort)axis_number;
    *(uint*)(node + axis_data_offset + 2) = link_data_offset;
    
    // Initialize link count to 0
    *(ushort*)(node + link_data_offset) = 0;
    
    // Update axis count
    (*axis_count)++;
    
    // Save changes to data.bin
    FILE* data_file = fopen(DATA_FILE, "r+b");
    if (data_file) {
        fseek(data_file, CoreMap[node_index].file_offset, SEEK_SET);
        fwrite(node, 1, 1 << (*(ushort*)node), data_file);
        fclose(data_file);
    }
    
    // Save updated free space information
    save_free_space();
    
    printf("Created axis %d in node %d, channel %d\n",
           axis_number, node_index, channel_index);
    
    return AXIS_SUCCESS;
}

int delete_axis(int node_index, int channel_index, int axis_number) {
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