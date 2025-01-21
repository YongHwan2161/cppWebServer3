#ifndef CGDB_H
#define CGDB_H

#include <stdio.h>
#include <stdlib.h>

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
} Coordinate;

extern uchar** Core;
extern nodeMapping* CoreMap;
extern int CoreSize;        // Current number of vertices loaded in Core
extern const unsigned int MaxCoreSize;  // Maximum number of vertices that can be in RAM
extern const unsigned int GarbagenodeIndex;

extern unsigned int CurrentnodeIndex;
extern unsigned int CurrentnodeCount;

extern const char* DATA_DIR;
extern const char* DATA_FILE;
extern const char* MAP_FILE;

extern Coordinate CurrentCoordinate;  // Current user position
extern Coordinate RootCoordinate;     // Root string position

#endif