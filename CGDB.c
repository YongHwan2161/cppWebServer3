#include "CGDB.h"
#include "src/init.h"
#include "src/database.h"
#include "src/free_space.h"
#include "src/axis.h"
#include "src/cli/command_handler.h"
#include <sys/stat.h>
#include <string.h>

uchar** Core;
NodeMapping* CoreMap;
int CoreSize = 0;
const int MaxCoreSize = 1024;  // Maximum nodes in RAM at once

const char* DATA_DIR = "binary-data";
const char* DATA_FILE = "binary-data/data.bin";
const char* MAP_FILE = "binary-data/map.bin";

int get_core_position(int node_index) {
    if (node_index >= 256) return -1;
    return CoreMap[node_index].core_position;
}

int load_node_to_core(int node_index) {
    if (CoreSize >= MaxCoreSize) {
        // Need to unload something first
        for (int i = 0; i < 256; i++) {
            if (CoreMap[i].is_loaded) {
                unload_node_from_core(i);
                break;
            }
        }
    }

    FILE* data_file = fopen(DATA_FILE, "rb");
    if (!data_file) return -1;
    
    // Use stored offset directly
    long offset = CoreMap[node_index].file_offset;
    
    // Load the node
    CoreMap[node_index].core_position = CoreSize;
    CoreMap[node_index].is_loaded = 1;
    load_node_from_file(data_file, offset, CoreSize);
    
    CoreSize++;
    fclose(data_file);
    return CoreMap[node_index].core_position;
}

void unload_node_from_core(int node_index) {
    int pos = CoreMap[node_index].core_position;
    if (pos < 0 || !CoreMap[node_index].is_loaded) return;
    
    // Free the node memory
    free(Core[pos]);
    
    // Shift remaining nodes
    for (int i = pos; i < CoreSize - 1; i++) {
        Core[i] = Core[i + 1];
    }
    
    // Update mapping for shifted nodes
    for (int i = 0; i < 256; i++) {
        if (CoreMap[i].core_position > pos) {
            CoreMap[i].core_position--;
        }
    }
    
    CoreMap[node_index].core_position = -1;
    CoreMap[node_index].is_loaded = 0;
    CoreSize--;
}

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