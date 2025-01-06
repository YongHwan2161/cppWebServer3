#include "CGDB.h"
#include <sys/stat.h>
#include <string.h>

uchar** Core;
const char* DATA_DIR = "binary-data";
const char* DATA_FILE = "binary-data/data.bin";
const char* MAP_FILE = "binary-data/map.bin";

uchar initValues[12] = {8, 0, 0, 0, 1, 0, 10, 0, 0, 0, 0, 0};

void create_new_node(int index) {
    uchar* newNode = (uchar*)malloc(12 * sizeof(uchar));
    for (int i = 0; i < 12; ++i) {
        newNode[i] = initValues[i];
    }
    Core[index] = newNode;
}

void create_DB() {
    printf("Creating new database...\n");
    Core = (uchar**)malloc(256 * sizeof(uchar*));
    for (int i = 0; i < 256; ++i) {
        create_new_node(i);
    }
}

void save_node_to_file(FILE* data_file, FILE* map_file, int index) {
    uchar* node = Core[index];
    long offset = ftell(data_file);
    
    fwrite(&offset, sizeof(long), 1, map_file);
    
    uint node_size = *(uint*)node;
    fwrite(node, sizeof(uchar), node_size + 4, data_file);
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
    // Move to the correct position in data file
    fseek(data_file, offset, SEEK_SET);
    
    // Read size first
    uint node_size;
    fread(&node_size, sizeof(uint), 1, data_file);
    
    // Allocate memory for the node
    uchar* newNode = (uchar*)malloc((node_size + 4) * sizeof(uchar));
    
    // Move back to read the whole node including size
    fseek(data_file, offset, SEEK_SET);
    fread(newNode, sizeof(uchar), node_size + 4, data_file);
    
    // Store in Core
    Core[index] = newNode;
}

void load_DB() {
    FILE* data_file = fopen(DATA_FILE, "rb");
    FILE* map_file = fopen(MAP_FILE, "rb");
    
    if (!data_file || !map_file) {
        printf("Error opening files for reading\n");
        return;
    }
    
    // Read number of nodes
    uint num_nodes;
    fread(&num_nodes, sizeof(uint), 1, map_file);
    
    // Allocate Core array
    Core = (uchar**)malloc(num_nodes * sizeof(uchar*));
    
    // Read each node's offset and load the node
    for (int i = 0; i < num_nodes; i++) {
        long offset;
        fread(&offset, sizeof(long), 1, map_file);
        load_node_from_file(data_file, offset, i);
    }
    
    fclose(data_file);
    fclose(map_file);
    printf("Database loaded successfully\n");
}

int main(int argc, char const *argv[]) {
    if (check_and_init_DB()) {
        printf("New database initialized\n");
    } else {
        printf("Using existing database\n");
    }
    return 0;
} 