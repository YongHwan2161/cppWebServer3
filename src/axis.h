#ifndef AXIS_H
#define AXIS_H

#include "../CGDB.h"
#include <stdbool.h>

// Axis operation results
#define AXIS_SUCCESS 0
#define AXIS_ERROR -1

// Common axis types
#define AXIS_FORWARD 0
#define AXIS_BACKWARD 1
#define AXIS_TIME 3

// Function declarations
int get_axis_count(uchar* node, ushort channel_index);
int get_axis_offset(uchar* node, ushort channel_index, ushort axis_number);
bool has_axis(uchar* node, uint channel_offset, ushort axis_number);
int create_axis(uint node_index, ushort channel_index, ushort axis_number);
int delete_axis(uint node_index, ushort channel_index, ushort axis_number);
uint get_last_axis_offset(uchar* node, ushort channel_index);

#endif 