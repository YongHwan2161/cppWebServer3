#include "database.h"
#include "init.h"
#include "Graph_structure/node.h"
#include "memory.h"
#include "Graph_structure/link.h"
#include <string.h>

#ifdef _WIN32
    #include <direct.h>
    #define MKDIR(dir) _mkdir(dir)
#else
    #include <sys/stat.h>
    #include <sys/types.h>
    #define MKDIR(dir) mkdir(dir, 0777)
#endif

int initialize_database() {
    // Check if map.bin exists
    FILE* map_file = fopen(MAP_FILE, "rb");
    FILE* data_file = fopen(DATA_FILE, "rb");
    
    if (!map_file || !data_file) {
        // Need to create new database
        if (map_file) fclose(map_file);
        if (data_file) fclose(data_file);
        
        create_DB();
        save_DB();
        return DB_NEW;
    }
    
    // Initialize CoreMap and load mapping information
    init_core_mapping();
    
    // Load initial set of nodes
    Core = (uchar**)malloc(MaxCoreSize * sizeof(uchar*));

    for (uint i = 0; i < MaxCoreSize && i < CurrentNodeCount; i++) {
        Core[i] = NULL;
        load_node_to_core(i);
    }
    
    fclose(map_file);
    fclose(data_file);
    return DB_SUCCESS;
}

void create_DB() {
    printf("Creating new database...\n");
    Core = (uchar**)malloc(MaxCoreSize * sizeof(uchar*));
    CoreMap = (NodeMapping*)malloc(257 * sizeof(NodeMapping));
    
    // Initialize CoreMap with default values
    for (int i = 0; i < 257; ++i) {
        CoreMap[i].core_position = i;
        CoreMap[i].is_loaded = 1;
        CoreMap[i].file_offset = 16 * i;  // Each node starts with 16 bytes, plus 4 bytes header        
        create_new_node();
    }
}

void save_DB() {
    // Create directory if it doesn't exist
    MKDIR(DATA_DIR);  // Using the macro instead of direct mkdir calls

    FILE* data_file = fopen(DATA_FILE, "wb");
    FILE* map_file = fopen(MAP_FILE, "wb");
    
    if (!data_file || !map_file) {
        printf("Error opening files for writing\n");
        return;
    }
    
    uint num_nodes = CurrentNodeCount;
    fwrite(&num_nodes, sizeof(uint), 1, map_file);
    
    for (uint i = 0; i < CurrentNodeCount; i++) {
        save_node_to_file2(data_file, map_file, i);
    }
    
    fclose(data_file);
    fclose(map_file);
    printf("Database saved successfully\n");
}
