#ifndef DATABASE_H
#define DATABASE_H

#include "../CGDB.h"

// Database status codes
#define DB_SUCCESS 0
#define DB_NEW 1
#define DB_ERROR -1

// Function declarations
int initialize_database(void);
void create_new_node(int index);
void create_DB(void);
void save_DB(void);
void load_node_from_file(FILE* data_file, long offset, uint index);
void load_DB(void);

#endif 