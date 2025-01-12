#include "database.h"
#include "init.h"
#include "Graph_structure/node.h"
#include "memory.h"
#include <string.h>

#ifdef _WIN32
    #include <direct.h>
    #define MKDIR(dir) _mkdir(dir)
#else
    #include <sys/stat.h>
    #include <sys/types.h>
    #define MKDIR(dir) mkdir(dir, 0777)
#endif

// Initial node values
static uchar initValues[16] = {
    4,  0,     // allocated size power (2^4 = 16 bytes)
    14, 0, 0, 0,  // actual used size (14 bytes initially)
    1,  0,     // number of channels (1)
    12, 0, 0, 0,   // offset for channel 0 (starts at byte 12)
    0,  0,     // number of axes (0)
    0,  0      // remaining bytes initialized to 0
};

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

    for (int i = 0; i < MaxCoreSize && i < 256; i++) {
        Core[i] = NULL;
        load_node_to_core(i);
    }
    
    fclose(map_file);
    fclose(data_file);
    return DB_SUCCESS;
}

void create_new_node(int index) {
    uchar* newNode = (uchar*)malloc(16 * sizeof(uchar));  // Always allocate 16 bytes initially
    for (int i = 0; i < 16; ++i) {
        newNode[i] = initValues[i];
    }
    Core[index] = newNode;
}

void create_DB() {
    printf("Creating new database...\n");
    Core = (uchar**)malloc(MaxCoreSize * sizeof(uchar*));
    CoreMap = (NodeMapping*)malloc(256 * sizeof(NodeMapping));
    
    // Initialize CoreMap with default values
    for (int i = 0; i < 256; ++i) {
        CoreMap[i].core_position = i;
        CoreMap[i].is_loaded = 1;
        CoreMap[i].file_offset = 16 * i;  // Each node starts with 16 bytes, plus 4 bytes header
        
        create_new_node(i);
        CoreSize++;
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
    
    uint num_nodes = 256;
    fwrite(&num_nodes, sizeof(uint), 1, map_file);
    
    for (int i = 0; i < 256; i++) {
        save_node_to_file2(data_file, map_file, i);
    }
    
    fclose(data_file);
    fclose(map_file);
    printf("Database saved successfully\n");
}

void load_DB() {
    Core = (uchar**)malloc(MaxCoreSize * sizeof(uchar*));
    
    // Initially load first MaxCoreSize nodes
    for (int i = 0; i < MaxCoreSize && i < 256; i++) {
        load_node_to_core(i);
    }
    init_core_mapping();
    printf("Database loaded successfully\n");
} 