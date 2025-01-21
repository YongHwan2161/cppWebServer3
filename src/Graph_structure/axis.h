#ifndef AXIS_H
#define AXIS_H

#include "../../CGDB.h"
#include <stdbool.h>

// Axis operation results
#define AXIS_SUCCESS 0
#define AXIS_ERROR -1


#define TOKEN_SEARCH_AXIS 0
#define TOKEN_DATA_AXIS 1
#define string_AXIS 2

// Function declarations
ushort get_axis_count(uchar* node, ushort channel_index);
uint get_axis_index(uchar* node, ushort channel_index, ushort axis_number);
uint get_axis_offset_by_index(uchar* node, ushort channel_index, ushort axis_index);
uint get_axis_offset(uchar* node, ushort channel_index, ushort axis_number);
bool has_axis(uchar* node, ushort channel_index, ushort axis_number);
bool has_axis_channel_offset(uchar* node, uint channel_offset, ushort axis_number);
bool ensure_axis_exists(uint node_index, ushort channel_index, ushort axis_number);
int create_axis(uint node_index, ushort channel_index, ushort axis_number);
int delete_axis(uint node_index, ushort channel_index, ushort axis_number);
uint get_last_axis_offset(uchar* node, ushort channel_index);

#endif 