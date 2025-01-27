#ifndef CGDB_H
#define CGDB_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ERROR -1
#define SUCCESS 0
// axis 정의
#define TOKEN_SEARCH_AXIS 0
#define TOKEN_DATA_AXIS 1
#define STRING_AXIS 2
#define PROPERTY_AXIS 3
#define CHILD_AXIS 4
#define PARENT_AXIS 5

// 문자열 시작 노드 node property
#define STRING_START_NODE 0

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

// Core status tracking
typedef struct {
    int core_position;   // Position in Core array (-1 if not loaded)
    int is_loaded;      // 1 if loaded in RAM, 0 if not
    long file_offset;   // Offset position in data.bin
} nodeMapping;

// Add after other global declarations
typedef struct {
    uint node;
    ushort channel;
} Vertex;

typedef struct {
    Vertex* vertices;
    int count;
} Vertices;


extern uchar** Core;
extern nodeMapping* CoreMap;
extern int CoreSize;        // Current number of vertices loaded in Core
extern int integrate_count;
extern const unsigned int MaxCoreSize;  // Maximum number of vertices that can be in RAM
extern const unsigned int init_node_count;

extern const unsigned int GarbagenodeIndex;
extern const unsigned int pointer_current_vertex;
extern const unsigned int pointer_root_vertex;

extern unsigned int CurrentnodeCount;
extern long max_offset;

extern const char* DATA_DIR;
extern const char* DATA_FILE;
extern const char* MAP_FILE;


extern Vertex CurrentVertex;  // Current user position
extern Vertex RootVertex;     // Root string position

extern bool sync_flag;

#endif