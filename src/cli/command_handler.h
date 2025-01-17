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
int handle_check_axis(char* args);
int handle_list_axes(char* args);
int handle_delete_axis(char* args);

int handle_create_link(char* args);
int handle_create_cycle(char* args);

int handle_create_channel(char* args);
int handle_clear_channel(char* args);

int handle_get_vertex_position(char* args);
int handle_load_vertex(char* args);


int handle_print_vertex(char* args);
int handle_print_free_space(char* args);
void print_help(void);

// Error handling utilities
void print_command_usage(const char* command, const char* args_desc);
void print_argument_error(const char* command, const char* args_desc, bool is_missing);

int handle_validate_free_offsets(char* args);
int handle_validate_cycle(char* args);

int handle_print_cycle(char* args);
int handle_print_garbage();

#endif // COMMAND_HANDLER_H 