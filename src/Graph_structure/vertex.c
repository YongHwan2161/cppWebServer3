#include "../../CGDB.h"
#include "vertex.h"
#include "axis.h"
#include "channel.h"
#include "link.h"
#include "../map.h"
#include "node/node.h"
#include <string.h>

int free_vertices(Vertices vertices) {
    if (vertices.vertices != NULL) {
        free(vertices.vertices);
        vertices.vertices = NULL;
        vertices.count = 0;
    }
    return SUCCESS;
}
// Helper function to get vertices connected through a specific axis
Vertices get_connected_vertices(Vertex vertex, ushort axis_number) {
    Vertices vertices;
    vertices.count = 0;
    vertices.vertices = NULL;

    if (!has_axis(Core[vertex.node], vertex.channel, axis_number)) {
        printf("Error: Axis %d does not exist in node %d, channel %d: get_connected_vertices()\n",
               axis_number, vertex.node, vertex.channel);
        return vertices;
    }

    uint node_position = CoreMap[vertex.node].core_position;
    uchar* node = Core[node_position];
    uint channel_offset = get_channel_offset(node, vertex.channel);
    uint axis_offset = get_axis_offset(node, vertex.channel, axis_number);
    ushort vertex_count = *(ushort*)(node + channel_offset + axis_offset);
    
    vertices.vertices = (Vertex*)malloc(sizeof(Vertex) * vertex_count);
    vertices.count = vertex_count;
    
    uint vertices_data_offset = channel_offset + axis_offset + 2;
    for (int i = 0; i < vertex_count; i++) {
        vertices.vertices[i] = (Vertex){
            *(uint*)(node + vertices_data_offset + (i * 6)),
            *(ushort*)(node + vertices_data_offset + (i * 6) + 4)
        };
    }
    
    return vertices;
}

Vertices get_parent_vertices(Vertex vertex) {
    return get_connected_vertices(vertex, PARENT_AXIS);
}

Vertices get_child_vertices(Vertex vertex) {
    return get_connected_vertices(vertex, CHILD_AXIS);
}

Vertex get_next_vertex(unsigned int node_index, unsigned short channel, unsigned short axis_number, ushort link_index) {
    long node_position = get_node_position(node_index);
    if (node_position == -1) return (Vertex){0, 0};
    uint channel_offset = get_channel_offset(Core[node_position], channel);
    uint axis_offset = get_axis_offset(Core[node_position], channel, axis_number);
    uint link_offset = channel_offset + axis_offset + 2;
    Vertex vertex;
    vertex.node = *(uint*)(Core[node_position] + link_offset + (link_index * 6));
    vertex.channel = *(ushort*)(Core[node_position] + link_offset + (link_index * 6) + 4);
    return vertex;
}
Vertex get_next_vertex_check(unsigned int node_index, unsigned short channel, unsigned short axis_number, ushort link_index) {
    if (!has_axis(Core[node_index], channel, axis_number)) return (Vertex){0, 0};
    if (get_link_count(node_index, channel, axis_number) < link_index + 1) {
        // printf("Error: Link index %d does not exist in node %d, channel %d, axis %d: get_next_vertex_check()\n", link_index, node_index, channel, axis_number);
        return (Vertex){0, 0};
    }
    Vertex next_vertex = get_next_vertex(node_index, channel, axis_number, link_index);
    if (next_vertex.node == node_index && next_vertex.channel == channel) return (Vertex){0, 0}; // loop vertex
    if (is_start_string_vertex(next_vertex)) return (Vertex){0, 0};
    return next_vertex;
}
int change_vertex(unsigned int node_index, unsigned int offset, Vertex vertex) {
    memcpy(Core[node_index] + offset, &vertex.node, sizeof(uint));
    memcpy(Core[node_index] + offset + 4, &vertex.channel, sizeof(ushort));
    if (!save_node_to_file(node_index)) {
        printf("Error: Failed to update data.bin\n");
        return ERROR;
    }
    return SUCCESS;
}
// Helper function to migrate vertices through a specific axis
static int migrate_vertices_through_axis(Vertex source_vertex, Vertex target_vertex, ushort source_axis, ushort target_axis, bool save) {
    long source_node_position = get_node_position(source_vertex.node);
    if (source_node_position == -1) return ERROR;
    if (!has_axis(Core[source_node_position], source_vertex.channel, source_axis)) {
        return ERROR;
    }
    Vertices source_vertices = get_connected_vertices(source_vertex, source_axis);
    if (source_vertices.vertices == NULL) {
        return ERROR;
    }
    for (int i = 0; i < source_vertices.count; i++) {
        long source_position = get_node_position(source_vertices.vertices[i].node);
        if (source_position == -1) return ERROR;
        uint source_channel_offset = get_channel_offset(Core[source_position], source_vertices.vertices[i].channel);
        uint source_target_axis_offset = get_axis_offset(Core[source_position], source_vertices.vertices[i].channel, target_axis);
        if (!has_axis(Core[source_position], source_vertices.vertices[i].channel, target_axis)) {
            return ERROR;
        }
        Vertices target_vertices = get_connected_vertices(source_vertices.vertices[i], target_axis);
        if (target_vertices.vertices == NULL) {
            free_vertices(source_vertices);
            return ERROR;
        }
        
        if (target_vertices.count > 0) {
            for (int j = 0; j < target_vertices.count; j++) {
                uint source_target_node = target_vertices.vertices[j].node;
                uint source_target_channel = target_vertices.vertices[j].channel;
                if (source_target_node == source_vertex.node && 
                    source_target_channel == source_vertex.channel) {
                        
                    change_vertex(source_position, source_channel_offset + source_target_axis_offset + 2 + (j * 6), target_vertex);
                    if (create_link(target_vertex.node, target_vertex.channel, 
                                source_vertices.vertices[i].node, 
                                source_vertices.vertices[i].channel, source_axis, save) != LINK_SUCCESS) {
                        printf("Error: Failed to create link between node %d and node %d\n", 
                               target_vertex.node, source_vertices.vertices[i].node);
                        return ERROR;
                    }
                    if (create_link(target_vertex.node, target_vertex.channel, 0, 0, PROPERTY_AXIS, save) != LINK_SUCCESS) {
                        printf("Error: Failed to create PROPERTY vertex of node %d\n", 
                               target_vertex.node);
                        return ERROR;
                    }
                    continue;
                }
            }
        }
        
        free_vertices(target_vertices);
    }
    
    free_vertices(source_vertices);
    return SUCCESS;
}

int migrate_parent_vertices(Vertex source_vertex, Vertex target_vertex, bool save) {
    return migrate_vertices_through_axis(source_vertex, target_vertex, PARENT_AXIS, CHILD_AXIS, save);
}

int migrate_child_vertices(Vertex source_vertex, Vertex target_vertex, bool save) {
    return migrate_vertices_through_axis(source_vertex, target_vertex, CHILD_AXIS, PARENT_AXIS, save);
}
bool is_start_string_vertex(Vertex vertex) {
    uchar* node = Core[vertex.node];
    if (has_axis(node, vertex.channel, PROPERTY_AXIS)) {
        uint channel_offset = get_channel_offset(node, vertex.channel);
        uint axis_offset = get_axis_offset(node, vertex.channel, PROPERTY_AXIS);
        Vertex property_vertex = (Vertex){*(uint*)(node + channel_offset + axis_offset + 2), 0};
        if (property_vertex.node == 0) {
            return true;
        }
    }
    return false;
}
bool is_root_vertex(Vertex vertex) {
    if (vertex.node == RootVertex.node && vertex.channel == RootVertex.channel) {
        return true;
    }
    return false;
}
int move_current_vertex(Vertex vertex) {
    CurrentVertex = vertex;
    update_current_vertex();
    return SUCCESS;
}
int handle_move_current_vertex(char* args) {
    Vertex vertex;
    sscanf(args, "%u %hu", &vertex.node, &vertex.channel);
    return move_current_vertex(vertex);
}
int load_current_vertex() {
    if (get_link(pointer_current_vertex, 0, PROPERTY_AXIS, 0, &CurrentVertex.node, &CurrentVertex.channel) == LINK_SUCCESS) {
        return SUCCESS;
    }
    return ERROR;
}
int load_root_vertex() {
    if (get_link(pointer_root_vertex, 0, PROPERTY_AXIS, 0, &RootVertex.node, &RootVertex.channel) == LINK_SUCCESS) {
        return SUCCESS;
    }
    return ERROR;
}
int update_current_vertex() {
    delete_first_link(pointer_current_vertex, 0, PROPERTY_AXIS, true);
    create_link(pointer_current_vertex, 0, CurrentVertex.node, CurrentVertex.channel, PROPERTY_AXIS, true);
    return SUCCESS;
}
int update_root_vertex() {
    delete_first_link(pointer_root_vertex, 0, PROPERTY_AXIS, true);
    create_link(pointer_root_vertex, 0, RootVertex.node, RootVertex.channel, PROPERTY_AXIS, true);
    return SUCCESS;
}
int update_current_vertex_to_root() {
    delete_first_link(pointer_current_vertex, 0, PROPERTY_AXIS, true);
    create_link(pointer_current_vertex, 0, RootVertex.node, RootVertex.channel, PROPERTY_AXIS, true);
    return SUCCESS;
}
int handle_get_current_vertex() {
    printf("Current vertex: node %u, channel %u\n", CurrentVertex.node, CurrentVertex.channel);
    return SUCCESS;
}
int handle_get_root_vertex() {
    printf("Root vertex: node %u, channel %u\n", RootVertex.node, RootVertex.channel);
    return SUCCESS;
}
