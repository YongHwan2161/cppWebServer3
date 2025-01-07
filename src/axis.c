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
    
    // Check if axis already exists
    if (has_axis(node, channel_offset, axis_number)) {
        printf("Warning: Axis %d already exists in node %d, channel %d\n",
               axis_number, node_index, channel_index);
        return AXIS_SUCCESS;  // Not an error, but no new axis created
    }
    
    // Get current axis count
    ushort* axis_count = (ushort*)(node + channel_offset);
    ushort current_axis_count = *axis_count;
    
    // Calculate new sizes
    ushort old_channel_size = get_channel_size(node, channel_index);
    ushort new_channel_size = old_channel_size + 6;  // Add 6 bytes for new axis
    
    // Check if we need to resize the node
    ushort node_size_power = *(ushort*)node;
    uint current_node_size = 1 << node_size_power;
    uint required_size = channel_offset + new_channel_size;
    
    if (required_size > current_node_size) {
        // Calculate new size (next power of 2)
        uint new_size = current_node_size;
        while (new_size < required_size) {
            new_size *= 2;
            node_size_power++;
        }
        
        // Try to find free space first
        FreeBlock* free_block = find_free_block(new_size);
        if (free_block) {
            // Copy node data to free block location
            uchar* new_node = (uchar*)malloc(new_size);
            memcpy(new_node, node, current_node_size);
            
            // Update CoreMap with new location
            CoreMap[node_index].file_offset = free_block->offset;
            
            // Add old space to free space
            add_free_block(current_node_size, CoreMap[node_index].file_offset);
            
            // Update node in Core
            free(node);
            node = new_node;
            Core[node_index] = new_node;
            
            // Update node size
            *(ushort*)node = node_size_power;
        } else {
            // No suitable free block found, allocate new space
            uchar* new_node = (uchar*)malloc(new_size);
            memcpy(new_node, node, current_node_size);
            
            // Add old space to free space
            add_free_block(current_node_size, CoreMap[node_index].file_offset);
            
            // Update CoreMap with new location (will be at end of file)
            FILE* data_file = fopen(DATA_FILE, "ab");
            if (data_file) {
                CoreMap[node_index].file_offset = ftell(data_file);
                fclose(data_file);
            }
            
            // Update node in Core
            free(node);
            node = new_node;
            Core[node_index] = new_node;
            
            // Update node size
            *(ushort*)node = node_size_power;
        }
    }
    
    // Update axis count
    (*axis_count)++;
    
    // Calculate offset for new axis data
    uint axis_data_offset = channel_offset + 2 + (current_axis_count * 6);
    
    // Write axis number and its data offset
    *(ushort*)(node + axis_data_offset) = (ushort)axis_number;
    *(uint*)(node + axis_data_offset + 2) = axis_data_offset + 6;  // Points to after itself
    
    // Save changes to data.bin
    FILE* data_file = fopen(DATA_FILE, "r+b");
    if (data_file) {
        fseek(data_file, CoreMap[node_index].file_offset, SEEK_SET);
        fwrite(node, 1, 1 << node_size_power, data_file);
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