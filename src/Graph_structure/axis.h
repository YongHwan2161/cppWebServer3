#ifndef AXIS_H
#define AXIS_H

#include "../../CGDB.h"
#include <stdbool.h>

// Axis operation results
#define AXIS_SUCCESS 0
#define AXIS_ERROR -1


#define TOKEN_SEARCH_AXIS 0
#define TOKEN_DATA_AXIS 1
#define SENTENCE_AXIS 2

// Function declarations
ushort get_axis_count(uchar* vertex, ushort channel_index);
uint get_axis_index(uchar* vertex, ushort channel_index, ushort axis_number);
uint get_axis_offset_by_index(uchar* vertex, ushort channel_index, ushort axis_index);
uint get_axis_offset(uchar* vertex, ushort channel_index, ushort axis_number);
bool has_axis(uchar* vertex, ushort channel_index, ushort axis_number);
bool has_axis_channel_offset(uchar* vertex, uint channel_offset, ushort axis_number);
bool ensure_axis_exists(uint vertex_index, ushort channel_index, ushort axis_number);
int create_axis(uint vertex_index, ushort channel_index, ushort axis_number);
int delete_axis(uint vertex_index, ushort channel_index, ushort axis_number);
uint get_last_axis_offset(uchar* vertex, ushort channel_index);

#endif 