#include "axis.h"
#include "channel.h"
#include "../free_space.h"
#include "link.h"
#include "vertex.h"
#include "../memory.h"
#include <string.h>
#include <stdbool.h>

ushort get_axis_count(uchar* vertex, ushort channel_index) {
    uint offset = get_channel_offset(vertex, channel_index);
    return *(ushort*)(vertex + offset);  // First 2 bytes contain axis count
}
uint get_axis_index(uchar* vertex, ushort channel_index, ushort axis_number) {
    uint channel_offset = get_channel_offset(vertex, channel_index);
    ushort axis_count = *(ushort*)(vertex + channel_offset);
    for (int i = 0; i < axis_count; i++) {
        if (*(ushort*)(vertex + channel_offset + 2 + (i * 6)) == axis_number) {
            return i;
        }
    }
    return 0;
}
uint get_axis_offset_by_index(uchar* vertex, ushort channel_index, ushort axis_index) {
    uint channel_offset = get_channel_offset(vertex, channel_index);
    return *(uint*)(vertex + channel_offset + 2 + (axis_index * 6) + 2);
}
uint get_axis_offset(uchar* vertex, ushort channel_index, ushort axis_number) {
    uint channel_offset = get_channel_offset(vertex, channel_index);
    
    ushort axis_count = *(ushort*)(vertex + channel_offset);
    int axis_data_offset = channel_offset + 2;  // Skip axis count
    
    // Search for the axis
    for (int i = 0; i < axis_count; i++) {
        ushort current_axis = *(ushort*)(vertex + axis_data_offset + (i * 6));
        if (current_axis == axis_number) {
            return *(uint*)(vertex + axis_data_offset + (i * 6) + 2);
        }
    }
    
    return 0;  // Axis not found
}

bool has_axis_channel_offset(uchar* vertex, uint channel_offset, ushort axis_number) {
    ushort axis_count = *(ushort*)(vertex + channel_offset);
    // printf("Axis count: %d\n", axis_count);
    uint axis_data_offset = channel_offset + 2;  // Skip axis count
    
    // Search for the axis
    for (ushort i = 0; i < axis_count; i++) {
        ushort current_axis = *(ushort*)(vertex + axis_data_offset + (i * 6));
        if (current_axis == axis_number) {
            return true;
        }
    }
    
    return false;
}
bool has_axis(uchar* vertex, ushort channel_index, ushort axis_number) {
    uint channel_offset = get_channel_offset(vertex, channel_index);
    return has_axis_channel_offset(vertex, channel_offset, axis_number);
}

int create_axis(uint vertex_index, ushort channel_index, ushort axis_number) {
    // Validate vertex
    if (!Core[vertex_index]) {
        printf("Error: Invalid vertex index\n");
        return AXIS_ERROR;
    }
    uint vertex_position = CoreMap[vertex_index].core_position;
    uchar* vertex = Core[vertex_position];
    uint channel_offset = get_channel_offset(vertex, channel_index);
        
    // Check if axis already exists
    if (has_axis_channel_offset(vertex, channel_offset, axis_number)) {
        printf("Error: Axis %d already exists\n", axis_number);
        return AXIS_ERROR;
    }
    // Get current actual size and calculate new required size
    uint current_actual_size = *(uint*)(vertex + 2);
    uint required_size = current_actual_size + 8;  // 6 bytes for axis entry + 2 bytes for link count
    
    // Check if we need to resize the vertex
    ushort vertex_size_power = *(ushort*)vertex;
    uint current_vertex_size = 1 << vertex_size_power;
    
    if (required_size > current_vertex_size) {
        uint new_size;
        uchar* new_vertex = resize_vertex_space(vertex, required_size, vertex_index, &new_size);
        if (!new_vertex) {
            printf("Error: Failed to resize vertex\n");
            return AXIS_ERROR;
        }
        Core[vertex_position] = new_vertex;
        // channel_offset remains the same, no need to recalculate
    }
    vertex = Core[vertex_position];
    // Get current axis count
    ushort current_count = *(ushort*)(vertex + channel_offset);
    uint insert_pos = channel_offset + 2 + (current_count * 6);
    uint new_axis_offset = 0;
    if (current_count == 0) {
        insert_pos = channel_offset + 2;
        new_axis_offset = 8;
    } else {
        uint last_axis_offset = get_last_axis_offset(vertex, channel_index);
        ushort current_link_count = *(ushort*)(vertex + channel_offset + last_axis_offset);
        new_axis_offset = last_axis_offset + 2 + (current_link_count * 6) + 6;  // -2 for new link count
    }
    uint move_size = required_size - insert_pos;
    insert_axis_entry(vertex, insert_pos, axis_number, new_axis_offset, move_size);

    //update channel offset
    ushort channel_count = *(ushort*)(vertex + 6);
    for (ushort i = channel_index + 1; i < channel_count; i++) {
        *(uint*)(vertex + 8 + (i * 4)) += 8;
    }
    //update axis offsets
    for (ushort i = 0; i < current_count; i++) {
        *(uint*)(vertex + channel_offset + 2 + (i * 6) + 2) += 6;
    }
    //insert new link count at the end of the axis data
    insert_pos = channel_offset + new_axis_offset;
    insert_ushort(vertex, insert_pos, 0, required_size - insert_pos);
    
    // Update axis count
    (*(ushort*)(vertex + channel_offset))++;
    
    // Update actual size
    *(uint*)(vertex + 2) = required_size;
    printf("before call save_vertex_to_file\n");
    if (!save_vertex_to_file(vertex_index)) {
        printf("Error: Failed to save vertex\n");
        return AXIS_ERROR;
    }
    printf("after call save_vertex_to_file\n");
    return AXIS_SUCCESS;
}

int delete_axis(uint vertex_index, ushort channel_index, ushort axis_number) {
    if (!Core[vertex_index]) return AXIS_ERROR;
    
    uchar* vertex = Core[vertex_index];
    uint channel_offset = get_channel_offset(vertex, channel_index);
    
    // Get current axis count
    ushort* axis_count = (ushort*)(vertex + channel_offset);
    
    // Find the axis to delete
    int axis_index = -1;
    uint axis_offset = 0;
    for (int i = 0; i < *axis_count; i++) {
        ushort current_axis = *(ushort*)(vertex + channel_offset + 2 + (i * 6));
        if (current_axis == axis_number) {
            axis_index = i;
            axis_offset = *(uint*)(vertex + channel_offset + 2 + (i * 6) + 2);
        }
        else {
            *(uint*)(vertex + channel_offset + 2 + (i * 6) + 2) -= 6;
        }
    }
    
    if (axis_index == -1) return AXIS_ERROR;
    
    // Calculate size to move
    uint current_actual_size = *(uint*)(vertex + 2);
    ushort link_count = *(ushort*)(vertex + channel_offset + axis_offset);
    uint move_start = channel_offset + axis_offset + 2 + (link_count * 6);
    uint move_dest = channel_offset + axis_offset;
    uint move_size = current_actual_size - move_start;
    memmove(vertex + move_dest,
            vertex + move_start,
            move_size);

    move_start = channel_offset + 2 + ((axis_index + 1) * 6);
    move_size = current_actual_size - move_start;
    memmove(vertex + move_start - 6,
            vertex + move_start,
            move_size);

    ushort channel_count = *(ushort*)(vertex + 6);
    uint size_reduction = 6 + 2 + (link_count * 6);
    for (ushort i = channel_index + 1; i < channel_count; i++) {
        *(uint*)(vertex + 8 + (i * 4)) -= size_reduction;
    }

    uint new_actual_size = current_actual_size - size_reduction;
    // Update axis count and actual size
    (*axis_count)--;
    *(uint*)(vertex + 2) = new_actual_size;  // -6 for removed axis entry
    
    // Save changes to data.bin
    if (!save_vertex_to_file(vertex_index)) {
        printf("Error: Failed to save vertex\n");
        return AXIS_ERROR;
    }
    return AXIS_SUCCESS;
}

uint get_last_axis_offset(uchar* vertex, ushort channel_index) {
    uint channel_offset = get_channel_offset(vertex, channel_index);
    ushort axis_count = *(ushort*)(vertex + channel_offset);
    
    if (axis_count == 0) {
        return 2;  // No axes exist
    }
    
    // Get offset of last axis from axis table
    uint axis_data_offset = channel_offset + 2;  // Skip axis count
    uint last_axis_offset = *(uint*)(vertex + axis_data_offset + ((axis_count - 1) * 6) + 2);
    
    return last_axis_offset;
}

/**
 * Checks if an axis exists, creates it if it doesn't
 * 
 * @param vertex_index Source vertex index
 * @param channel_index Channel index
 * @param axis_number Axis number to check/create
 * @return AXIS_SUCCESS if axis exists or was created successfully, AXIS_ERROR otherwise
 */
bool ensure_axis_exists(uint vertex_index, ushort channel_index, ushort axis_number) {

    uchar* vertex = Core[vertex_index];
    // Check if axis exists
    if (!has_axis(vertex, channel_index, axis_number)) {
        int result = create_axis(vertex_index, channel_index, axis_number);
        if (result != AXIS_SUCCESS) {
            printf("Error: Failed to create required axis\n");
            return false;
        }
    }

    return true;
} 