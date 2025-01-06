#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <stdbool.h>

// Command result codes
#define CMD_SUCCESS 0
#define CMD_ERROR -1
#define CMD_EXIT 1

// Command handler function
int handle_command(char* command);

// Individual command handlers
int handle_create_axis(char* args);
void print_help(void);

#endif // COMMAND_HANDLER_H 