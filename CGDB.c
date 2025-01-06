#include "CGDB.h"
#include "src/init.h"
#include <sys/stat.h>
#include <string.h>

uchar** Core;
NodeMapping* CoreMap;
int CoreSize = 0;
const int MaxCoreSize = 1024;  // Maximum nodes in RAM at once

const char* DATA_DIR = "binary-data";
const char* DATA_FILE = "binary-data/data.bin";
const char* MAP_FILE = "binary-data/map.bin";

uchar initValues[16] = {
    4,  0,     // data size (2^4 = 16 bytes)
    1,  0,     // number of channels (1)
    8,  0, 0, 0,   // offset for channel 0 (starts at byte 8)
    0,  0,     // number of axes (0)
    0,  0, 0, 0, 0, 0    // remaining bytes initialized to 0
};

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
    #ifdef _WIN32
        _mkdir(DATA_DIR);
    #else
        mkdir(DATA_DIR, 0777);
    #endif

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

int check_and_init_DB() {
    FILE* data_file = fopen(DATA_FILE, "rb");
    FILE* map_file = fopen(MAP_FILE, "rb");
    
    if (!data_file || !map_file) {
        if (data_file) fclose(data_file);
        if (map_file) fclose(map_file);
        
        // Files don't exist, create new database
        create_DB();
        save_DB();
        return 1;  // New database created
    }
    
    fclose(data_file);
    fclose(map_file);
    
    // Load existing database
    load_DB();
    return 0;  // Existing database loaded
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

int main() {
    int init_status = initialize_system();
    
    if (init_status == INIT_ERROR) {
        printf("System initialization failed\n");
        return 1;
    } else if (init_status == INIT_NEW_DB) {
        printf("New database initialized\n");
    } else {
        printf("Using existing database\n");
    }
    
    // Program logic here...
    
    cleanup_system();
    return 0;
} 