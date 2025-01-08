#ifndef MAP_H
#define MAP_H

#include "../CGDB.h"

// Map operation results
#define MAP_SUCCESS 0
#define MAP_ERROR -1

// Function declarations
int save_map(uint node_index);
int save_map_all(void);
int load_map(void);
void init_map(void);

#endif // MAP_H 