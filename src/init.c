#include "init.h"
#include "free_space.h"
#include "database.h"
#include "../CGDB.h"
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
    CoreMap = (NodeMapping*)malloc(MaxCoreSize * sizeof(NodeMapping));
    
    // Initialize with default values
    for (int i = 0; i < MaxCoreSize; i++) {
        CoreMap[i].core_position = -1;
        CoreMap[i].is_loaded = 0;
        CoreMap[i].file_offset = 0;
    }
    
    // Read offsets from map.bin
    FILE* map_file = fopen(MAP_FILE, "rb");
    if (map_file) {
        // Skip number of nodes
        fread(&CoreSize, sizeof(uint), 1, map_file);
        fseek(map_file, sizeof(uint), SEEK_SET);
        
        // Read all offsets
        for (int i = 0; i < CoreSize; i++) {
            fread(&CoreMap[i].file_offset, sizeof(long), 1, map_file);
        }
        fclose(map_file);
    } else {
        printf("Error: Failed to open map.bin\n");
    }
}

int initialize_system() {
    // 1. Check and create binary-data directory
    if (check_and_create_directory() != 0) {
        printf("Error creating data directory\n");
        return INIT_ERROR;
    }
    
    // 2. Initialize database (creates or loads existing)
    int db_status = initialize_database();
    if (db_status == DB_ERROR) {
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
    if (db_status == DB_NEW || fs_status == FREE_SPACE_NEW) {
        return DB_NEW;
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