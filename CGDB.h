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
} NodeMapping;

extern uchar** Core;
extern NodeMapping* CoreMap;
extern int CoreSize;        // Current number of nodes loaded in Core
extern const unsigned int MaxCoreSize;  // Maximum number of nodes that can be in RAM
extern const unsigned int GarbageNodeIndex;
extern unsigned int CurrentNodeCount;

extern const char* DATA_DIR;
extern const char* DATA_FILE;
extern const char* MAP_FILE;

#endif 