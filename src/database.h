#ifndef DATABASE_H
#define DATABASE_H

#include "../CGDB.h"

// Database status codes
#define DB_SUCCESS 0
#define DB_NEW 1
#define DB_ERROR -1

// Function declarations
int initialize_database(void);
void create_DB(void);
long get_last_file_offset();
#endif 