#include "../../CGDB.h"
#include "vertex.h"
#include "axis.h"
#include "channel.h"
#include "link.h"
#include "../map.h"
int free_vertices(Vertices vertices) {
    if (vertices.vertices) {
        free(vertices.vertices);
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

// Helper function to migrate vertices through a specific axis
static int migrate_vertices_through_axis(Vertex source_vertex, Vertex target_vertex, ushort source_axis, ushort target_axis) {
    Vertices source_vertices = get_connected_vertices(source_vertex, source_axis);
    // uint target_position = get_node_position(target_vertex.node);
    
    for (int i = 0; i < source_vertices.count; i++) {
        uint source_position = get_node_position(source_vertices.vertices[i].node);
        uint source_channel_offset = get_channel_offset(Core[source_position], source_vertices.vertices[i].channel);
        uint source_target_axis_offset = get_axis_offset(Core[source_position], source_vertices.vertices[i].channel, target_axis);
        
        Vertices target_vertices = get_connected_vertices(source_vertices.vertices[i], target_axis);
        if (target_vertices.count > 0) {
            for (int j = 0; j < target_vertices.count; j++) {
                uint source_target_node = target_vertices.vertices[j].node;
                uint source_target_channel = target_vertices.vertices[j].channel;
                if (source_target_node == source_vertex.node && 
                    source_target_channel == source_vertex.channel) {
                    // *(Vertex*)(Core[source_position] + source_channel_offset + source_target_axis_offset + 2 + (j * 6)) = target_vertex;
                    
                    *(uint*)(Core[source_position] + source_channel_offset + source_target_axis_offset + 2 + (j * 6)) = target_vertex.node;
                    *(ushort*)(Core[source_position] + source_channel_offset + source_target_axis_offset + 2 + (j * 6) + 4) = target_vertex.channel;
                    if (create_link(target_vertex.node, target_vertex.channel, 
                                source_vertices.vertices[i].node, 
                                source_vertices.vertices[i].channel, source_axis) != LINK_SUCCESS) {
                        printf("Error: Failed to create link between node %d and node %d\n", 
                               target_vertex.node, source_vertices.vertices[i].node);
                        return ERROR;
                    }
                    if (create_link(target_vertex.node, target_vertex.channel, 0, 0, PROPERTY_AXIS) != LINK_SUCCESS) {
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

int migrate_parent_vertices(Vertex source_vertex, Vertex target_vertex) {
    return migrate_vertices_through_axis(source_vertex, target_vertex, PARENT_AXIS, CHILD_AXIS);
}

int migrate_child_vertices(Vertex source_vertex, Vertex target_vertex) {
    return migrate_vertices_through_axis(source_vertex, target_vertex, CHILD_AXIS, PARENT_AXIS);
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
