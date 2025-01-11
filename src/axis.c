#include "axis.h"
#include "channel.h"
#include "free_space.h"
#include "link.h"
#include "node.h"
#include <string.h>
#include <stdbool.h>

ushort get_axis_count(uchar* node, ushort channel_index) {
    uint offset = get_channel_offset(node, channel_index);
    return *(ushort*)(node + offset);  // First 2 bytes contain axis count
}

uint get_axis_offset(uchar* node, ushort channel_index, ushort axis_number) {
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
    
    return 0;  // Axis not found
}

bool has_axis_channel_offset(uchar* node, uint channel_offset, ushort axis_number) {
    ushort axis_count = *(ushort*)(node + channel_offset);
    // printf("Axis count: %d\n", axis_count);
    uint axis_data_offset = channel_offset + 2;  // Skip axis count
    
    // Search for the axis
    for (ushort i = 0; i < axis_count; i++) {
        ushort current_axis = *(ushort*)(node + axis_data_offset + (i * 6));
        if (current_axis == axis_number) {
            return true;
        }
    }
    
    return false;
}
bool has_axis(uchar* node, ushort channel_index, ushort axis_number) {
    uint channel_offset = get_channel_offset(node, channel_index);
    return has_axis_channel_offset(node, channel_offset, axis_number);
}

int create_axis(uint node_index, ushort channel_index, ushort axis_number) {
    // Validate node
    if (!Core[node_index]) {
        printf("Error: Invalid node index\n");
        return AXIS_ERROR;
    }
    
    uchar* node = Core[node_index];
    uint channel_offset = get_channel_offset(node, channel_index);
        
    // Check if axis already exists
    if (has_axis_channel_offset(node, channel_offset, axis_number)) {
        printf("Error: Axis %d already exists\n", axis_number);
        return AXIS_ERROR;
    }
    // Get current actual size and calculate new required size
    uint current_actual_size = *(uint*)(node + 2);
    uint required_size = current_actual_size + 8;  // 6 bytes for axis entry + 2 bytes for link count
    
    // Check if we need to resize the node
    ushort node_size_power = *(ushort*)node;
    uint current_node_size = 1 << node_size_power;
    
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
    
    // Get current axis count
    ushort current_count = *(ushort*)(node + channel_offset);

    
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
    
    // Update actual size
    *(uint*)(node + 2) = required_size;
    
    if (!save_node_to_file(node_index)) {
        printf("Error: Failed to save node\n");
        return AXIS_ERROR;
    }
    return AXIS_SUCCESS;
}

int delete_axis(uint node_index, ushort channel_index, ushort axis_number) {
    if (!Core[node_index]) return AXIS_ERROR;
    
    uchar* node = Core[node_index];
    uint channel_offset = get_channel_offset(node, channel_index);
    
    // Get current axis count
    ushort* axis_count = (ushort*)(node + channel_offset);
    
    // Find the axis to delete
    int axis_index = -1;
    uint axis_offset = 0;
    uint next_axis_offset = 0;
    
    for (int i = 0; i < *axis_count; i++) {
        ushort current_axis = *(ushort*)(node + channel_offset + 2 + (i * 6));
        if (current_axis == axis_number) {
            axis_index = i;
            axis_offset = *(uint*)(node + channel_offset + 2 + (i * 6) + 2);
            if (i < *axis_count - 1) {
                next_axis_offset = *(uint*)(node + channel_offset + 2 + ((i + 1) * 6) + 2);
            }
            break;
        }
    }
    
    if (axis_index == -1) return AXIS_ERROR;
    
    // Calculate size to move
    uint current_actual_size = *(uint*)(node + 2);
    uint move_size;
    uint size_reduction;
    
    if (axis_index == *axis_count - 1) {
        // Last axis - just update actual size
        size_reduction = current_actual_size - (channel_offset + axis_offset);
    } else {
        // Middle axis - need to move data
        move_size = current_actual_size - (channel_offset + next_axis_offset);
        memmove(node + channel_offset + axis_offset,
                node + channel_offset + next_axis_offset,
                move_size);
                
        size_reduction = next_axis_offset - axis_offset;
        
        // Update remaining axis offsets
        for (int i = axis_index + 1; i < *axis_count; i++) {
            uint* offset_ptr = (uint*)(node + channel_offset + 2 + (i * 6) + 2);
            *offset_ptr -= size_reduction;
        }
    }
    
    // Remove axis entry from table
    memmove(node + channel_offset + 2 + (axis_index * 6),
            node + channel_offset + 2 + ((axis_index + 1) * 6),
            (*axis_count - axis_index - 1) * 6);
    
    // Update axis count and actual size
    (*axis_count)--;
    *(uint*)(node + 2) = current_actual_size - size_reduction - 6;  // -6 for removed axis entry
    
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

/**
 * Checks if an axis exists, creates it if it doesn't
 * 
 * @param node_index Source node index
 * @param channel_index Channel index
 * @param axis_number Axis number to check/create
 * @return AXIS_SUCCESS if axis exists or was created successfully, AXIS_ERROR otherwise
 */
bool ensure_axis_exists(uint node_index, ushort channel_index, ushort axis_number) {

    uchar* node = Core[node_index];
    // Check if axis exists
    if (!has_axis(node, channel_index, axis_number)) {
        int result = create_axis(node_index, channel_index, axis_number);
        if (result != AXIS_SUCCESS) {
            printf("Error: Failed to create required axis\n");
            return false;
        }
    }

    return true;
} 