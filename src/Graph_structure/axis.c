#include "axis.h"
#include "channel.h"
#include "../free_space.h"
#include "link.h"
#include "node.h"
#include "../memory.h"
#include "../map.h"
#include <string.h>
#include <stdbool.h>

ushort get_axis_count(uchar* node, ushort channel_index) {
    uint offset = get_channel_offset(node, channel_index);
    return *(ushort*)(node + offset);  // First 2 bytes contain axis count
}
uint get_axis_index(uchar* node, ushort channel_index, ushort axis_number) {
    uint channel_offset = get_channel_offset(node, channel_index);
    ushort axis_count = *(ushort*)(node + channel_offset);
    for (int i = 0; i < axis_count; i++) {
        if (*(ushort*)(node + channel_offset + 2 + (i * 6)) == axis_number) {
            return i;
        }
    }
    return 0;
}
uint get_axis_offset_by_index(uchar* node, ushort channel_index, ushort axis_index) {
    uint channel_offset = get_channel_offset(node, channel_index);
    return *(uint*)(node + channel_offset + 2 + (axis_index * 6) + 2);
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
    uint node_position = get_node_position(node_index);
    // Validate node
    if (!Core[node_position]) {
        printf("Error: Invalid node index\n");
        return AXIS_ERROR;
    }
    uchar* node = Core[node_position];
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
        uchar* new_node = resize_node_space(node, required_size, node_index, &new_size);
        if (!new_node) {
            printf("Error: Failed to resize node\n");
            return AXIS_ERROR;
        }
        Core[node_position] = new_node;
        // channel_offset remains the same, no need to recalculate
    }
    node = Core[node_position];
    // Get current axis count
    ushort current_count = *(ushort*)(node + channel_offset);
    uint insert_pos = channel_offset + 2 + (current_count * 6);
    uint new_axis_offset = 0;
    if (current_count == 0) {
        insert_pos = channel_offset + 2;
        new_axis_offset = 8;
    } else {
        uint last_axis_offset = get_last_axis_offset(node, channel_index);
        ushort current_link_count = *(ushort*)(node + channel_offset + last_axis_offset);
        new_axis_offset = last_axis_offset + 2 + (current_link_count * 6) + 6;  // -2 for new link count
    }
    uint move_size = required_size - insert_pos;
    insert_axis_entry(node, insert_pos, axis_number, new_axis_offset, move_size);

    //update channel offset
    ushort channel_count = *(ushort*)(node + 6);
    for (ushort i = channel_index + 1; i < channel_count; i++) {
        *(uint*)(node + 8 + (i * 4)) += 8;
    }
    //update axis offsets
    for (ushort i = 0; i < current_count; i++) {
        *(uint*)(node + channel_offset + 2 + (i * 6) + 2) += 6;
    }
    //insert new link count at the end of the axis data
    insert_pos = channel_offset + new_axis_offset;
    insert_ushort(node, insert_pos, 0, required_size - insert_pos);
    
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
    uint node_position = get_node_position(node_index);
    if (!Core[node_position]) return AXIS_ERROR;
    
    uchar* node = Core[node_position];
    uint channel_offset = get_channel_offset(node, channel_index);
    
    // Get current axis count
    ushort* axis_count = (ushort*)(node + channel_offset);
    
    // Find the axis to delete
    int axis_index = -1;
    uint axis_offset = 0;
    for (int i = 0; i < *axis_count; i++) {
        ushort current_axis = *(ushort*)(node + channel_offset + 2 + (i * 6));
        if (current_axis == axis_number) {
            axis_index = i;
            axis_offset = *(uint*)(node + channel_offset + 2 + (i * 6) + 2);
        }
        else {
            *(uint*)(node + channel_offset + 2 + (i * 6) + 2) -= 6;
        }
    }
    
    if (axis_index == -1) return AXIS_ERROR;
    
    // Calculate size to move
    uint current_actual_size = *(uint*)(node + 2);
    ushort link_count = *(ushort*)(node + channel_offset + axis_offset);
    uint move_start = channel_offset + axis_offset + 2 + (link_count * 6);
    uint move_dest = channel_offset + axis_offset;
    uint move_size = current_actual_size - move_start;
    memmove(node + move_dest,
            node + move_start,
            move_size);

    move_start = channel_offset + 2 + ((axis_index + 1) * 6);
    move_size = current_actual_size - move_start;
    memmove(node + move_start - 6,
            node + move_start,
            move_size);

    ushort channel_count = *(ushort*)(node + 6);
    uint size_reduction = 6 + 2 + (link_count * 6);
    for (ushort i = channel_index + 1; i < channel_count; i++) {
        *(uint*)(node + 8 + (i * 4)) -= size_reduction;
    }

    uint new_actual_size = current_actual_size - size_reduction;
    // Update axis count and actual size
    (*axis_count)--;
    *(uint*)(node + 2) = new_actual_size;  // -6 for removed axis entry
    
    // Save changes to data.bin
    if (!save_node_to_file(node_index)) {
        printf("Error: Failed to save node\n");
        return AXIS_ERROR;
    }
    return AXIS_SUCCESS;
}
int delete_property_axis(uint node_index, ushort channel_index) {
    delete_axis(node_index, channel_index, PROPERTY_AXIS);
    return AXIS_SUCCESS;
}
uint get_last_axis_offset(uchar* node, ushort channel_index) {
    uint channel_offset = get_channel_offset(node, channel_index);
    ushort axis_count = *(ushort*)(node + channel_offset);
    
    if (axis_count == 0) {
        return 2;  // No axes exist
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
    uint node_position = get_node_position(node_index);
    uchar* node = Core[node_position];
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
