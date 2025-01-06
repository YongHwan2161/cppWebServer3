#ifndef CHANNEL_H
#define CHANNEL_H

#include "../CGDB.h"

// Channel status codes
#define CHANNEL_SUCCESS 0
#define CHANNEL_ERROR -1

// Function declarations
ushort get_channel_count(uchar* node);
uint get_channel_offset(uchar* node, int channel_index);
ushort get_channel_size(uchar* node, int channel_index);
int create_channel(int node_index);

#endif 