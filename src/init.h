#ifndef INIT_H
#define INIT_H

#include "../CGDB.h"
#include "free_space.h"

// Initialization status codes
#define INIT_SUCCESS 0
#define INIT_NEW_DB 1
#define INIT_ERROR -1

// Function declarations
void init_core_mapping();
int initialize_system(void);
int check_and_create_directory(void);
int initialize_database(void);
void cleanup_system(void);

#endif 