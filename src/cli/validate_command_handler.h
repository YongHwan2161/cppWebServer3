#ifndef VALIDATE_COMMAND_HANDLER_H
#define VALIDATE_COMMAND_HANDLER_H

#include <stdbool.h>

// Validates that free block offsets are unique and non-overlapping
int validate_free_offsets(char* args);

#endif // VALIDATE_COMMAND_HANDLER_H 