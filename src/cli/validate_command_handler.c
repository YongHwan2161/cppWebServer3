#include "validate_command_handler.h"
#include "../Graph_structure/axis.h"
#include "../Graph_structure/channel.h"
#include "../Graph_structure/link.h"
#include "../free_space.h"
#include "../memory.h"
#include "command_handler.h"
#include "../tests/axis_tests.h"
#include "../map.h"
#include "../Graph_structure/cycle.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_VISITED_vertices 1000

typedef struct {
    uint vertex;
    ushort channel;
    ushort axis;
} Pathvertex;

int validate_free_offsets(char* args) {
    if (args) {
        print_argument_error("validate-free-offsets", "", false);
        return CMD_ERROR;
    }
    
    int failed = test_free_block_offsets();
    return (failed == 0) ? CMD_SUCCESS : CMD_ERROR;
}

bool validate_cycle(int vertex_index, int channel_index, int axis_number) {
    cycleInfo* info = get_cycle_info(vertex_index, channel_index, axis_number);
    bool has_cycle = info->count > 0;
    
    if (has_cycle) {
        printf("Found cycle with %d vertices:\n", info->count);
        for (int i = 0; i < info->count; i++) {
            printf("vertex %u, Channel %u\n", info->vertices[i], info->channels[i]);
        }
    }
    
    free_cycle_info(info);
    return has_cycle;
}
