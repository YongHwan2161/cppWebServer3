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
        // save_DB();
        return DB_NEW;
    } else {
        printf("Database already exists\n");
        // Initialize CoreMap and load mapping information
        init_core_mapping();

        // Load initial set of vertices
        Core = (uchar **)malloc(MaxCoreSize * sizeof(uchar *));

        for (uint i = 0; i < CurrentnodeCount; i++)
        {
            Core[i] = NULL;
            load_node_to_core(i);
        }

        fclose(map_file);
        fclose(data_file);
        return DB_SUCCESS;
    }
}

void create_DB() {
    printf("Creating new database...\n");
    Core = (uchar**)malloc(MaxCoreSize * sizeof(uchar*));
    CoreMap = (nodeMapping*)malloc(257 * sizeof(nodeMapping));
    
    // Initialize CoreMap with default values
    for (int i = 0; i < 256; ++i) {
        CoreMap[i].core_position = i;
        CoreMap[i].is_loaded = 1;
        CoreMap[i].file_offset = 16 * i;  // Each node starts with 16 bytes, plus 4 bytes header        
        create_new_node();
    }
    // create_axis(256, 0, 0); // create axis first
    // create_loop(GarbagenodeIndex, 0, 0);
}
