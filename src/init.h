#ifndef INIT_H
#define INIT_H


// Initialization status codes
#define INIT_SUCCESS 0
// #define INIT_NEW_DB 1
#define INIT_ERROR -1

// Function declarations
void init_core_mapping();
int initialize_system(void);
int check_and_create_directory(void);
void cleanup_system(void);

#endif 