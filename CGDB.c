#include "CGDB.h"
#include "src/init.h"
#include "src/database.h"
#include "src/free_space.h"
#include "src/Graph_structure/axis.h"
#include "src/cli/command_handler.h"
#include "src/memory.h"
#include <sys/stat.h>
#include <string.h>

uchar** Core;
NodeMapping* CoreMap;
int CoreSize = 0;
const unsigned int MaxCoreSize = 1024;  // Maximum nodes in RAM at once
unsigned int CurrentNodeCount = 0;

const char* DATA_DIR = "binary-data";
const char* DATA_FILE = "binary-data/data.bin";
const char* MAP_FILE = "binary-data/map.bin";


int main() {
    int init_status = initialize_system();
    
    if (init_status == INIT_ERROR) {
        printf("System initialization failed\n");
        return 1;
    } else if (init_status == DB_NEW) {
        printf("New database initialized\n");
    } else {
        printf("Using existing database\n");
    }
    
    printf("CGDB Command Line Interface\n");
    printf("Type 'help' for available commands\n");
    
    char command[256];
    while (1) {
        printf("\n> ");
        if (!fgets(command, sizeof(command), stdin)) {
            break;
        }
        
        int result = handle_command(command);
        if (result == CMD_EXIT) {
            break;
        }
    }
    
    cleanup_system();
    return 0;
} 