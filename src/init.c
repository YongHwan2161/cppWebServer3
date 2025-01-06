#include "init.h"
#include "free_space.h"
#include <sys/stat.h>
#include <string.h>

int check_and_create_directory() {
    struct stat st = {0};
    
    if (stat(DATA_DIR, &st) == -1) {
        #ifdef _WIN32
            return _mkdir(DATA_DIR);
        #else
            return mkdir(DATA_DIR, 0777);
        #endif
    }
    return 0;
}
void init_core_mapping() {
    CoreMap = (NodeMapping*)malloc(256 * sizeof(NodeMapping));
    
    // Initialize with default values
    for (int i = 0; i < 256; i++) {
        CoreMap[i].core_position = -1;
        CoreMap[i].is_loaded = 0;
        CoreMap[i].file_offset = 0;
    }
    
    // Read offsets from map.bin
    FILE* map_file = fopen(MAP_FILE, "rb");
    if (map_file) {
        // Skip number of nodes
        fseek(map_file, sizeof(uint), SEEK_SET);
        
        // Read all offsets
        for (int i = 0; i < 256; i++) {
            fread(&CoreMap[i].file_offset, sizeof(long), 1, map_file);
        }
        fclose(map_file);
    }
}
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
        return INIT_NEW_DB;
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
    return INIT_SUCCESS;
}

int initialize_system() {
    // 1. Check and create binary-data directory
    if (check_and_create_directory() != 0) {
        printf("Error creating data directory\n");
        return INIT_ERROR;
    }
    
    // 2. Initialize database (creates or loads existing)
    int db_status = initialize_database();
    if (db_status == INIT_ERROR) {
        printf("Error initializing database\n");
        return INIT_ERROR;
    }
    
    // 3. Initialize free space management
    int fs_status = init_free_space();
    if (fs_status == FREE_SPACE_ERROR) {
        printf("Error initializing free space management\n");
        return INIT_ERROR;
    }
    
    // Return NEW if either database or free space was newly created
    if (db_status == INIT_NEW_DB || fs_status == FREE_SPACE_NEW) {
        return INIT_NEW_DB;
    }
    
    return INIT_SUCCESS;
}

void cleanup_system() {
    // Free CoreMap
    if (CoreMap) {
        free(CoreMap);
        CoreMap = NULL;
    }
    
    // Free Core and its contents
    if (Core) {
        for (int i = 0; i < CoreSize; i++) {
            if (Core[i]) free(Core[i]);
        }
        free(Core);
        Core = NULL;
    }
    
    // Free FreeSpace
    if (free_space) {
        if (free_space->blocks) free(free_space->blocks);
        if (free_space->free_indices) free(free_space->free_indices);
        free(free_space);
        free_space = NULL;
    }
} 