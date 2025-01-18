#include "link.h"
#include "channel.h"
#include "../free_space.h"
#include "vertex.h"
#include "../memory.h"
#include <stdio.h>
#include <string.h>

int create_link(uint source_vertex, ushort source_ch,
                uint dest_vertex, ushort dest_ch,
                ushort axis_number)
{
    if (!ensure_axis_exists(source_vertex, source_ch, axis_number))
    {
        printf("Error: Axis %d does not exist in vertex %d, channel %d\n",
               axis_number, source_vertex, source_ch);
        return LINK_ERROR;
    }
    uint vertex_position = CoreMap[source_vertex].core_position;
    uchar* vertex = Core[vertex_position];
    uint channel_offset = get_channel_offset(vertex, source_ch);
    uint axis_offset = get_axis_offset(vertex, source_ch, axis_number);
    uint current_actual_size = *(uint*)(vertex + 2);
    uint required_size = current_actual_size + 6;  // Add 6 bytes for new link
    // Check and resize if needed using the new function
    int resize_result = check_and_resize_vertex(vertex, required_size, source_vertex);
    if (resize_result == FREE_SPACE_ERROR) {
        printf("Error: Failed to resize vertex\n");
        return LINK_ERROR;
    }
    vertex = Core[vertex_position];
    
    ushort channel_count = get_channel_count(vertex);  // Get channel count
    ushort* current_link_count = (ushort*)(vertex + channel_offset + axis_offset);
    uint link_insert_offset = channel_offset + axis_offset + 2 + (*current_link_count * 6);
    uint move_size = current_actual_size - link_insert_offset;  // Move all remaining data
    insert_link(vertex, link_insert_offset, dest_vertex, dest_ch, move_size);
    // Update offsets in current channel
    uint axis_entry_offset = channel_offset + 2;
    ushort axis_count = get_axis_count(vertex, source_ch);
    for (ushort i = 0; i < axis_count; i++)
    {
        uint current_axis_offset = *(uint *)(vertex + axis_entry_offset + (i * 6) + 2);
        if (current_axis_offset > axis_offset)
        {
            *(uint *)(vertex + axis_entry_offset + (i * 6) + 2) += 6;
        }
    }
    // Update only channel offsets for subsequent channels
    if (source_ch < channel_count - 1)
    {
        for (ushort ch = source_ch + 1; ch < channel_count; ch++)
        {
            *(uint *)(vertex + 8 + (ch * 4)) += 6;
        }
    }

    // Update link count and actual size
    (*current_link_count)++;
    *(uint*)(vertex + 2) = required_size;
    
    if (!save_vertex_to_file(source_vertex)) {
        printf("Error: Failed to update data.bin\n");
        return LINK_ERROR;
    }
    return LINK_SUCCESS;
}
int create_forward_link(uint source_vertex, ushort source_ch,
                        uint dest_vertex, ushort dest_ch) {
    create_link(source_vertex, source_ch, dest_vertex, dest_ch, 0);
    return LINK_SUCCESS;
}
int create_backward_link(uint source_vertex, ushort source_ch,
                        uint dest_vertex, ushort dest_ch) {
    create_link(source_vertex, source_ch, dest_vertex, dest_ch, 1);
    return LINK_SUCCESS;
}
/**
 * @brief Create a loop link between a vertex and itself
 * 
 * @param source_vertex The vertex to create the loop link from
 * @param source_ch The channel to create the loop link on
 * @param axis_number The axis number to create the loop link on
 * @return LINK_SUCCESS if the loop link is created successfully, LINK_ERROR if it fails
 * make sure to create the axis first
 */
int create_loop(uint source_vertex, ushort source_ch, ushort axis_number) {
    create_link(source_vertex, source_ch, source_vertex, source_ch, axis_number);
    return LINK_SUCCESS;
}
int create_bidirectional_link(uint source_vertex, ushort source_ch, 
                        uint dest_vertex, ushort dest_ch) {
    create_forward_link(source_vertex, source_ch, dest_vertex, dest_ch);
    create_backward_link(dest_vertex, dest_ch, source_vertex, source_ch);
    return LINK_SUCCESS;
}
int delete_link(uint source_vertex, ushort source_ch, 
               uint dest_vertex, ushort dest_ch, 
               ushort axis_number) {
    
    uchar* vertex = Core[source_vertex];
    uint channel_offset = get_channel_offset(vertex, source_ch);
    ushort axis_index = get_axis_index(vertex, source_ch, axis_number);
    // Get axis offset and link count
    uint axis_offset = get_axis_offset_by_index(vertex, source_ch, axis_index);
    ushort* current_link_count = (ushort*)(vertex + channel_offset + axis_offset);
    uint link_data_offset = channel_offset + axis_offset + 2;  // Skip link count
    // Search for the link
    bool found = false;
    int link_position = -1;
    
    for (int i = 0; i < *current_link_count; i++) {
        Link* current_link = (Link*)(vertex + link_data_offset + (i * 6));
        if (current_link->vertex == dest_vertex && current_link->channel == dest_ch) {
            found = true;
            link_position = i;
            break;
        }
    }
    
    if (!found) {
        printf("Error: Link not found\n");
        return LINK_ERROR;
    }
    
    uint target_link_offset = link_data_offset + (link_position * 6);
    uint actual_size = *(uint*)(vertex + 2);
    // Shift remaining links to fill the gap
    if (target_link_offset < actual_size - 6) {
        memmove(vertex + target_link_offset,
                vertex + target_link_offset + 6,
                actual_size - target_link_offset - 6);
    }
    uint current_axis_count = get_axis_count(vertex, source_ch);
    uint axis_entry_offset = channel_offset + 2;
    for (ushort i = axis_index; i < current_axis_count; i++) {
        uint current_axis_offset = *(uint *)(vertex + axis_entry_offset + (i * 6) + 2);
        if (current_axis_offset > axis_offset) {
            *(uint *)(vertex + axis_entry_offset + (i * 6) + 2) -= 6;
        }
    }
    ushort channel_count = get_channel_count(vertex);
    for (ushort ch = source_ch + 1; ch < channel_count; ch++) {
        *(uint *)(vertex + 8 + (ch * 4)) -= 6;
    }
    // Update link count and actual size
    (*current_link_count)--;
    *(uint*)(vertex + 2) = actual_size - 6;
    
    if (!save_vertex_to_file(source_vertex)) {
        printf("Error: Failed to update data.bin\n");
        return LINK_ERROR;
    }
    return LINK_SUCCESS;
} 