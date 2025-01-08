#ifndef CGDB_H
#define CGDB_H

#include <stdio.h>
#include <stdlib.h>

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

// Core status tracking
typedef struct {
    int core_position;   // Position in Core array (-1 if not loaded)
    int is_loaded;      // 1 if loaded in RAM, 0 if not
    long file_offset;   // Offset position in data.bin
} NodeMapping;

extern uchar** Core;
extern NodeMapping* CoreMap;
extern int CoreSize;        // Current number of nodes loaded in Core
extern const int MaxCoreSize;  // Maximum number of nodes that can be in RAM

extern const char* DATA_DIR;
extern const char* DATA_FILE;
extern const char* MAP_FILE;

// Add new function declarations
int get_core_position(int node_index);
int load_node_to_core(int node_index);
void unload_node_from_core(int node_index);

// Existing function declarations
void create_new_node(int index);
void create_DB();
void save_node_to_file(FILE* data_file, FILE* map_file, int index);
void save_DB();
// int check_and_init_DB();
void load_node_from_file(FILE* data_file, long offset, int index);
void load_DB();

#endif 