#include "database.h"
#include "init.h"
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
    4,  0,     // data size (2^4 = 16 bytes)
    1,  0,     // number of channels (1)
    8,  0, 0, 0,   // offset for channel 0 (starts at byte 8)
    0,  0,     // number of axes (0)
    0,  0, 0, 0, 0, 0    // remaining bytes initialized to 0
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
    init_core_mapping();
    
    for (int i = 0; i < 256; ++i) {
        create_new_node(i);
        CoreMap[i].core_position = i;
        CoreMap[i].is_loaded = 1;
        CoreSize++;
    }
}

void save_node_to_file(FILE* data_file, FILE* map_file, int index) {
    uchar* node = Core[index];
    long offset = ftell(data_file);
    
    fwrite(&offset, sizeof(long), 1, map_file);
    
    // Read size as power of 2 (first 2 bytes)
    ushort size_power = *(ushort*)node;
    uint actual_size = 1 << size_power;  // 2^size_power
    
    // Write the entire node data
    fwrite(node, sizeof(uchar), actual_size, data_file);
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
        save_node_to_file(data_file, map_file, i);
    }
    
    fclose(data_file);
    fclose(map_file);
    printf("Database saved successfully\n");
}

void load_node_from_file(FILE* data_file, long offset, int index) {
    fseek(data_file, offset, SEEK_SET);
    
    // Read size power first (2 bytes)
    ushort size_power;
    fread(&size_power, sizeof(ushort), 1, data_file);
    
    // Calculate actual size
    uint actual_size = 1 << size_power;
    
    // Allocate memory for the node
    uchar* newNode = (uchar*)malloc(actual_size * sizeof(uchar));
    
    // Move back 2 bytes instead of seeking from start
    fseek(data_file, -2, SEEK_CUR);
    fread(newNode, sizeof(uchar), actual_size, data_file);
    
    Core[index] = newNode;
}

void load_DB() {
    Core = (uchar**)malloc(MaxCoreSize * sizeof(uchar*));
    init_core_mapping();
    
    // Initially load first MaxCoreSize nodes
    for (int i = 0; i < MaxCoreSize && i < 256; i++) {
        load_node_to_core(i);
    }
    printf("Database loaded successfully\n");
} 