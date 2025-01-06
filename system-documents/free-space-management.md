# Free Space Management

## Overview
Free space management is crucial for efficient database operations, particularly when nodes are deleted or resized. The system maintains a record of available spaces in the data file and deleted node indices for reuse.

## Free Space Structure
```c
typedef struct {
    uint size;          // Size of free block (power of 2)
    long offset;        // Offset in data.bin
} FreeBlock;

typedef struct {
    uint count;         // Number of free blocks
    FreeBlock* blocks;  // Array of free blocks
    uint* free_indices; // Array of deleted node indices
    uint index_count;   // Number of free indices
} FreeSpace;
```

## Free Space Operations

### 1. Free Space Creation
- When nodes are deleted
- When nodes are resized and need larger space
- Original space is marked as free

### 2. Free Space Reduction
- When new nodes are created
- When existing free space is reused
- When deleted indices are reused

### 3. File Synchronization
Three binary files must be synchronized:
- data.bin: Node data storage
- map.bin: Node offset mapping
- free_space.bin: Free space information

## Implementation Details

### Free Space File Format
```
[Block Count (4 bytes)]
[Free Blocks (count * sizeof(FreeBlock))]
[Free Index Count (4 bytes)]
[Free Indices (index_count * sizeof(uint))]
```

### Key Functions
1. `init_free_space()`: Initialize/load free space data
2. `save_free_space()`: Save free space information
3. `find_free_block()`: Find suitable free block
4. `add_free_block()`: Add new free block
5. `get_free_index()`: Get available node index
6. `add_free_index()`: Add deleted node index
7. `release_node_space()`: Release node space

### Memory Management
- Free space information is kept in RAM during program execution
- Changes are immediately written to free_space.bin
- Synchronization is maintained with data.bin and map.bin

### Space Allocation Strategy
1. Check for available free block of required size
2. If found, use the free block
3. If not found, allocate at end of file
4. Always maintain power-of-2 sizes 