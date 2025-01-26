#ifndef CHANNEL_H
#define CHANNEL_H

#include "../../CGDB.h"
#include <stdbool.h>
// Channel status codes
#define CHANNEL_SUCCESS 0
#define CHANNEL_ERROR -1

// Function declarations
ushort get_channel_count(uchar* node);
uint get_channel_offset(uchar* node, ushort channel_index);
uint get_channel_end_offset(uchar* node, ushort channel_index);
int create_channel(uint node_index, bool sync);
int recycle_or_create_channel(uint node_index, bool sync);
int clear_channel(uint node_index, ushort channel_index, bool sync);
int clear_channels(uint *node_index, ushort *channel_index, ushort count, bool sync);
int create_multi_channels(uint node_index, int num_channels, bool sync);
#endif