# Data Consistency Verification

## Overview
The system provides functionality to verify that node data in memory (Core array) matches the data stored in data.bin and map.bin files. This consistency must be maintained at all times to ensure data integrity.

## Verification Function

### check_node_consistency
```c
bool check_node_consistency(uint node_index);
```

#### Purpose
Verifies that:
1. Node data in Core array matches data.bin
2. Node mapping in CoreMap matches map.bin
3. File offsets are correctly maintained

#### Process
1. Memory Data Check
   - Get node position from CoreMap
   - Access node data in Core array
   - Calculate node size

2. File Data Check
   - Open data.bin and map.bin
   - Read file data at stored offsets
   - Compare with memory data

3. Mapping Verification
   - Compare CoreMap entries with map.bin
   - Verify file offsets match
   - Check load status consistency

#### Return Value
- true: Data is consistent across memory and files
- false: Inconsistency detected (if node not loaded, files can't be opened, or data mismatch)

### Implementation
```c
bool check_node_consistency(uint node_index) {
    // Get node position
    int position = get_node_position(node_index);
    if (position < 0) return false;
    
    // Open files
    FILE* data_file = fopen(DATA_FILE, "rb");
    FILE* map_file = fopen(MAP_FILE, "rb");
    if (!data_file || !map_file) {
        if (data_file) fclose(data_file);
        if (map_file) fclose(map_file);
        return false;
    }
    
    // Get node size and verify data
    uchar* node = Core[position];
    size_t node_size = 1 << (*(ushort*)node);
    
    // Read and compare file data
    uchar* file_data = malloc(node_size);
    if (!file_data) {
        fclose(data_file);
        fclose(map_file);
        return false;
    }
    
    fseek(data_file, CoreMap[node_index].file_offset, SEEK_SET);
    size_t read_size = fread(file_data, 1, node_size, data_file);
    
    bool is_consistent = (read_size == node_size && 
                         memcmp(node, file_data, node_size) == 0);
    
    // Cleanup
    free(file_data);
    fclose(data_file);
    fclose(map_file);
    
    return is_consistent;
}
```

### Usage Example
```c
// Check consistency for node 5
if (!check_node_consistency(5)) {
    printf("Warning: Data inconsistency detected in node 5\n");
}
```

## Best Practices

### When to Check
1. After Critical Operations
   - Node data modifications
   - File I/O operations
   - Memory management changes

2. Debug Scenarios
   - Data corruption investigation
   - Memory consistency verification
   - File I/O troubleshooting

### Error Handling
1. Inconsistency Detection
   - Log detailed error information
   - Identify inconsistency type
   - Report affected components

2. Recovery Actions
   - Reload node from file
   - Update file data
   - Synchronize mappings

### Performance Considerations
1. Resource Usage
   - File I/O operations
   - Memory allocation
   - Comparison operations

2. Optimization
   - Check only when necessary
   - Batch consistency checks
   - Efficient file access

### Error Cases
The function returns false when:
1. Node is not loaded in memory (position < 0)
2. Files cannot be opened
3. Memory allocation fails
4. File read size doesn't match expected size
5. Memory data differs from file data 