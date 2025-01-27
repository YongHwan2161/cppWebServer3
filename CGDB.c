#include "CGDB.h"
#include "src/init.h"
#include "src/database.h"
#include "src/free_space.h"
#include "src/Graph_structure/axis.h"
#include "src/Graph_structure/node/node.h"
#include "src/Graph_structure/vertex.h"
#include "src/cli/command_handler.h"
#include "src/memory.h"
#include <sys/stat.h>
#include <string.h>

uchar** Core;
nodeMapping* CoreMap;
int CoreSize = 0;
const unsigned int MaxCoreSize = 32768;  // Maximum vertices in RAM at once
const unsigned int init_node_count = 259;

const unsigned int GarbagenodeIndex = 256;
const unsigned int pointer_current_vertex = 257;
const unsigned int pointer_root_vertex = 258;
// const unsigned int pointer_max_offset_node = 259;

long max_offset = 0;
unsigned int CurrentnodeCount = 0;
int integrate_count = 2;

const char* DATA_DIR = "binary-data";
const char* DATA_FILE = "binary-data/data.bin";
const char* MAP_FILE = "binary-data/map.bin";

bool sync_flag = true;

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

    load_current_vertex();
    load_root_vertex();

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