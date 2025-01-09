#include "axis.h"
#include "channel.h"
#include "free_space.h"
#include "link.h"
#include <string.h>
#include <stdbool.h>

int get_axis_count(uchar* node, ushort channel_index) {
    uint offset = get_channel_offset(node, channel_index);
    return *(ushort*)(node + offset);  // First 2 bytes contain axis count
}

int get_axis_offset(uchar* node, ushort channel_index, ushort axis_number) {
    uint channel_offset = get_channel_offset(node, channel_index);
    
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
    ushort current_count = *(ushort*)(node + channel_offset);
    
    // Check if axis already exists
    if (has_axis(node, channel_offset, axis_number)) {
        printf("Error: Axis %d already exists\n", axis_number);
        return AXIS_ERROR;
    }
    
    // Calculate new axis table size
    uint axis_table_size = (current_count + 1) * 6;  // Including new axis entry
    
    // Calculate required size
    uint required_size;
    if (current_count == 0) {
        required_size = channel_offset + axis_table_size + 4;  // +2 for axis count, +2 for link count
    } else {
        // Get last axis's offset and data size
        uint* last_offset_ptr = (uint*)(node + channel_offset + 2 + ((current_count - 1) * 6) + 2);
        uint last_axis_offset = *last_offset_ptr;
        ushort* last_link_count = (ushort*)(node + channel_offset + last_axis_offset);
        uint last_axis_data_size = 2 + (*last_link_count * sizeof(Link));
        
        required_size = channel_offset + last_axis_offset + last_axis_data_size + 6 + 2;
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
        // channel_offset remains the same, no need to recalculate
    }
    
    // Move existing axis data forward
    if (current_count > 0) {
        uint data_start = channel_offset + 2 + (current_count * 6);
        uint data_size = current_node_size - data_start - 6;  // Entire remaining data
        
        // Move all axis data forward by 6 bytes
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
    uint new_axis_offset;
    if (current_count == 0) {
        new_axis_offset = 8;  // Fixed offset: axis count(2) + first axis table entry(6)
    } else {
        // Use already calculated required_size
        new_axis_offset = required_size - channel_offset - 2;  // -2 for new link count
    }
    
    ushort* new_axis_number = (ushort*)(node + channel_offset + 2 + (current_count * 6));
    uint* new_axis_offset_ptr = (uint*)(node + channel_offset + 2 + (current_count * 6) + 2);
    
    *new_axis_number = axis_number;
    *new_axis_offset_ptr = new_axis_offset;
    
    // Initialize new axis data (link count = 0)
    ushort* new_link_count = (ushort*)(node + channel_offset + new_axis_offset);
    *new_link_count = 0;
    
    // Update axis count
    (*(ushort*)(node + channel_offset))++;
    
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
    
    // Get current axis count and find target axis position
    ushort* axis_count = (ushort*)(node + channel_offset);
    int axis_position = -1;
    uint axis_data_offset = channel_offset + 2;  // Skip axis count
    
    for (int i = 0; i < *axis_count; i++) {
        if (*(ushort*)(node + axis_data_offset + (i * 6)) == axis_number) {
            axis_position = i;
            break;
        }
    }
    
    // Calculate size to remove
    uint target_axis_offset = *(uint*)(node + axis_data_offset + (axis_position * 6) + 2);
    ushort* target_link_count = (ushort*)(node + channel_offset + target_axis_offset);
    uint bytes_to_remove = 6 + 2 + (*target_link_count * 6);  // axis entry + link count + links
    
    // If not the last axis, move later axis data forward
    if (axis_position < *axis_count - 1) {
        // Calculate start of next axis
        uint next_axis_start = channel_offset + target_axis_offset + 2 + (*target_link_count * 6);
        uint data_to_move_size;
        
        if (axis_position == *axis_count - 2) {
            // Moving last axis
            uint last_axis_offset = *(uint*)(node + axis_data_offset + ((*axis_count - 1) * 6) + 2);
            ushort* last_link_count = (ushort*)(node + channel_offset + last_axis_offset);
            data_to_move_size = 2 + (*last_link_count * 6);  // link count + links
        } else {
            // Moving multiple axes
            uint last_axis_offset = *(uint*)(node + axis_data_offset + ((*axis_count - 1) * 6) + 2);
            ushort* last_link_count = (ushort*)(node + channel_offset + last_axis_offset);
            data_to_move_size = (channel_offset + last_axis_offset + 2 + (*last_link_count * 6)) - next_axis_start;
        }
        
        // Move data forward
        memmove(node + channel_offset + target_axis_offset,
                node + next_axis_start,
                data_to_move_size);
        
        // Update offsets for remaining axes
        for (int i = axis_position + 1; i < *axis_count; i++) {
            uint* offset_ptr = (uint*)(node + axis_data_offset + (i * 6) + 2);
            *offset_ptr -= bytes_to_remove;
        }
        
        // Remove axis entry
        memmove(node + axis_data_offset + (axis_position * 6),
                node + axis_data_offset + ((axis_position + 1) * 6),
                (*axis_count - axis_position - 1) * 6);
    }
    
    // Update axis count
    (*axis_count)--;
    
    // Save changes to data.bin
    FILE* data_file = fopen(DATA_FILE, "r+b");
    if (data_file) {
        fseek(data_file, CoreMap[node_index].file_offset, SEEK_SET);
        fwrite(node, 1, 1 << (*(ushort*)node), data_file);
        fclose(data_file);
        return AXIS_SUCCESS;
    }
    
    printf("Error: Failed to update data.bin\n");
    return AXIS_ERROR;
}

uint get_last_axis_offset(uchar* node, ushort channel_index) {
    uint channel_offset = get_channel_offset(node, channel_index);
    ushort axis_count = *(ushort*)(node + channel_offset);
    
    if (axis_count == 0) {
        return -1;  // No axes exist
    }
    
    // Get offset of last axis from axis table
    uint axis_data_offset = channel_offset + 2;  // Skip axis count
    uint last_axis_offset = *(uint*)(node + axis_data_offset + ((axis_count - 1) * 6) + 2);
    
    return last_axis_offset;
} 