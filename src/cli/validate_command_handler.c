#include "validate_command_handler.h"
#include "../Graph_structure/axis.h"
#include "../Graph_structure/channel.h"
#include "../Graph_structure/link.h"
#include "../free_space.h"
#include "../memory.h"
#include "command_handler.h"
#include "../tests/axis_tests.h"
#include "../map.h"
#include "../Graph_structure/circle.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_VISITED_NODES 1000

typedef struct {
    uint node;
    ushort channel;
    ushort axis;
} PathNode;

int validate_free_offsets(char* args) {
    if (args) {
        print_argument_error("validate-free-offsets", "", false);
        return CMD_ERROR;
    }
    
    int failed = test_free_block_offsets();
    return (failed == 0) ? CMD_SUCCESS : CMD_ERROR;
}

bool validate_circle(int node_index, int channel_index, int axis_number) {
    CircleInfo* info = get_circle_info(node_index, channel_index, axis_number);
    bool has_circle = info->count > 0;
    
    if (has_circle) {
        printf("Found circle with %d nodes:\n", info->count);
        for (int i = 0; i < info->count; i++) {
            printf("Node %u, Channel %u\n", info->nodes[i], info->channels[i]);
        }
    }
    
    free_circle_info(info);
    return has_circle;
}
