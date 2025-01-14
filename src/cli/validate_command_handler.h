#ifndef VALIDATE_COMMAND_HANDLER_H
#define VALIDATE_COMMAND_HANDLER_H

#include <stdbool.h>

// Validates that free block offsets are unique and non-overlapping
int validate_free_offsets(char* args);

// Validates if there is a circle starting from given vertex/channel/axis
bool validate_circle(int vertex_index, int channel_index, int axis_number);

#endif // VALIDATE_COMMAND_HANDLER_H 