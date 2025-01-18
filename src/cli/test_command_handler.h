#ifndef TEST_COMMAND_HANDLER_H
#define TEST_COMMAND_HANDLER_H

#include "command_handler.h"

// Test command handlers
int handle_run_tests(char* args);
int handle_test_resize(char* args);
int handle_test_axis_create_delete(char* args);
int handle_test_free_offsets(char* args);
int handle_test_multiple_link_creation(char* args);
int handle_test_create_delete_links(char* args);
int handle_test_multi_channel_links(char* args);
int handle_test_channel_creation(char* args);
#endif // TEST_COMMAND_HANDLER_H 
