#ifndef CGDB_H
#define CGDB_H

#include <stdio.h>
#include <stdlib.h>

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

extern uchar** Core;
extern const char* DATA_DIR;
extern const char* DATA_FILE;
extern const char* MAP_FILE;

void create_new_node(int index);
void create_DB();
void save_node_to_file(FILE* data_file, FILE* map_file, int index);
void save_DB();
int check_and_init_DB();
void load_node_from_file(FILE* data_file, long offset, int index);
void load_DB();

#endif 