#include "database.h"
#include "init.h"
#include "Graph_structure/node/node.h"
#include "Graph_structure/link.h"
#include "Graph_structure/cycle.h"
#include "Graph_structure/vertex.h"
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
        max_offset = get_last_file_offset();
        fclose(map_file);
        fclose(data_file);
        return DB_SUCCESS;
    }
}

void create_DB() {
    printf("Creating new database...\n");
    Core = (uchar**)malloc(MaxCoreSize * sizeof(uchar*));
    // CoreMap = (nodeMapping*)malloc(259 * sizeof(nodeMapping));
    
    // Initialize CoreMap with default values
    for (unsigned int i = 0; i < init_node_count; ++i) {
        CoreMap[i].core_position = i;
        CoreMap[i].is_loaded = 1;
        CoreMap[i].file_offset = 16 * i;  // Each node starts with 16 bytes, plus 4 bytes header        
        create_new_node();
    }
    create_axis(256, 0, 0, true); // create axis first
    create_loop(GarbagenodeIndex, 0, 0, true);
            // Create root string "root"
    uint start_node;
    ushort start_channel;
        if (handle_create_string("root", &start_node, &start_channel, true, false, true) != SUCCESS) {
            printf("Error: Failed to create root string\n");
            return;
        }
    for (unsigned int i = 0; i < 3; i++)
    {
        if (handle_create_string("root", &start_node, &start_channel, false, true, true) != SUCCESS) {
            printf("Error: Failed to create root string\n");
            return;
        }
    }
    
    // Store root vertexs
    RootVertex = (Vertex){start_node, start_channel};
    // Set current position to root
    CurrentVertex = RootVertex;
    update_current_vertex();
    update_root_vertex();
    
    printf("Root string created at node %u, channel %u\n", 
           RootVertex.node, RootVertex.channel);
             
}
long get_last_file_offset() {
    FILE* data_file = fopen(DATA_FILE, "rb");
    if (!data_file) {
        return 0;  // If file doesn't exist, start at 0
    }
    
    // Get file size
    fseek(data_file, 0, SEEK_END);
    long file_size = ftell(data_file);
    fclose(data_file);
    
    return file_size;
}
int handle_max_offset(void) {
    printf("Max offset: %ld\n", max_offset);
    return SUCCESS;
}