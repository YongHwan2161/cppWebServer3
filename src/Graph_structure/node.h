#ifndef NODE_H
#define NODE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
// Function declarations
bool validate_node(unsigned int node_index);
bool save_node_to_file(unsigned int node_index);
bool save_node_to_file2(FILE* data_file, FILE* map_file, unsigned int index);
void create_new_node();

#endif // NODE_H 