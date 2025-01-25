# Cycle Management

[Previous content remains...]

## Path Insertion

### Overview
The system provides functionality to insert a path of vertices into an existing cycle, creating a longer cycle. This allows dynamic modification of cycles while maintaining their integrity.

### Process
1. Input Validation
   - Verify node indices
   - Validate channel existence
   - Check axis number

2. Channel Validation
   - Verify each path node is loaded
   - Check channel exists in each node
   - Ensure channel indices are valid

3. Cycle Breaking
   - Break cycle at insertion point
   - Preserve cycle structure
   - Maintain link references

4. Path Integration
   - Connect path start to break point
   - Link path vertices sequentially
   - Connect path end to cycle

### Command Interface
```shell
insert-path <node> <channel> <axis> <path_vertices_and_channels...>
```

#### Parameters
- node: Insertion point node index
- channel: Insertion point channel index
- axis: Axis number for the cycle
- path_vertices_and_channels: Alternating node and channel indices for path

#### Examples
```shell
# Insert 2-node path into cycle
> insert-path 42 1 2 55 3 66 4
Successfully inserted path of length 2 into cycle

# Error cases
> insert-path 42 1 2
Error: No path vertices provided

> insert-path 42 1 2 55
Error: Missing channel for node 55

# Channel doesn't exist
> insert-path 42 1 2 55 10 66 4
Error: Channel 10 does not exist in node 55 (max: 2)

# node not loaded
> insert-path 42 1 2 255 1 66 4
Error: node 255 not loaded
```

### Implementation Details

#### Path Insertion Process
```c
// Break cycle at insertion point
delete_link(insert_node, insert_channel, next_node, next_channel, axis_number);

// Link path start to insertion point
create_link(insert_node, insert_channel,
           path_vertices[0], path_channels[0],
           axis_number);

// Create links along path
for (int i = 0; i < path_length - 1; i++) {
    create_link(path_vertices[i], path_channels[i],
               path_vertices[i + 1], path_channels[i + 1],
               axis_number);
}

// Link path end to next cycle node
create_link(path_vertices[path_length - 1], path_channels[path_length - 1],
           next_node, next_channel,
           axis_number);
```

### Notes
1. Cycle Integrity
   - Maintains cycle structure
   - Preserves existing connections
   - Validates insertion point

2. Path Handling
   - Sequential path linking
   - Channel preservation
   - Proper node ordering

3. Error Handling
   - Invalid insertion point
   - Missing path data
   - Link creation failures

4. Performance
   - Minimal cycle disruption
   - Efficient path integration
   - Clean error recovery 

## Path Deletion

### Overview
The system provides functionality to delete a path of vertices from an existing cycle, creating a shorter cycle. This allows dynamic modification of cycles while maintaining their integrity.

### Process
1. Cycle Validation
   - Verify existing cycle at deletion point
   - Check path length validity
   - Ensure cycle is long enough

2. Path Removal
   - Break cycle at path boundaries
   - Clear channels in removed path
   - Reconnect cycle ends

3. Channel Cleanup
   - Clear removed channels
   - Reset axis counts
   - Remove all links

### Command Interface
```shell
delete-path <node> <channel> <axis> <path_length>
```

#### Parameters
- node: Starting node index
- channel: Starting channel index
- axis: Axis number for the cycle
- path_length: Number of vertices to remove

#### Examples
```shell
# Delete 2-node path from cycle
> delete-path 42 1 2 2
Successfully deleted path of length 2 from cycle

# Error cases
> delete-path 42 1 2 0
Error: Invalid path length

> delete-path 42 1 2 5
Error: Cycle too short for requested path length
```

### Implementation Details

#### Path Deletion Process
```c
// Break cycle at path boundaries
delete_link(start_node, start_channel, first_path_node, first_path_channel, axis);
delete_link(last_path_node, last_path_channel, next_node, next_channel, axis);

// Create new link to close cycle
create_link(start_node, start_channel, next_node, next_channel, axis);

// Clear channels in removed path
for (each node in path) {
    clear_channel(node, channel);
}
```

### Notes
1. Cycle Integrity
   - Maintains cycle structure
   - Preserves remaining connections
   - Validates operation feasibility

2. Path Cleanup
   - Complete channel clearing
   - All links removed
   - Axis counts reset

3. Error Handling
   - Invalid path length
   - Cycle too short
   - Link operation failures

4. Performance
   - Minimal cycle disruption
   - Efficient path removal
   - Clean channel cleanup 

## String Cycle Optimization

### Overview
The system provides functionality to optimize string cycles by combining adjacent tokens where possible, reducing the overall cycle length while maintaining the same string data.

### Command Interface
```shell
optimize-string <node> <channel> <axis>
```

#### Parameters
- node: Index of node containing the cycle
- channel: Channel index
- axis: Axis number (usually 2 for string cycles)

#### Process
1. Cycle Analysis
   - Get cycle information
   - Verify minimum length (2 vertices)
   - Check token compatibility

2. Token Combination
   - Compare adjacent tokens
   - Create combined tokens
   - Update cycle structure

3. Cycle Update
   - Replace old tokens with combined tokens
   - Maintain cycle integrity
   - Preserve string data

#### Examples
```shell
# Optimize string cycle
> optimize-string 42 1 2
Successfully optimized string cycle

# Error cases
> optimize-string 42
Error: Missing arguments
Usage: optimize-string <node> <channel> <axis>

> optimize-string 42 1 2
Error: No cycle found at node 42 channel 1 axis 2
```

### Implementation Details

#### Optimization Process
```c
int optimize_string_cycle(uint node_index, ushort channel, ushort axis) {
    // Get cycle info
    cycleInfo* cycle = get_cycle_info(node_index, channel, axis);
    
    // Process adjacent vertices
    for (int i = 0; i < cycle->count - 1; i++) {
        // Compare and combine tokens
        // Update cycle structure
    }
}
```

### Notes
1. Performance
   - Reduces cycle length
   - Minimizes token count
   - Optimizes traversal

2. Data Preservation
   - Maintains string content
   - Preserves token relationships
   - Ensures cycle integrity

3. Error Handling
   - Invalid cycle detection
   - Token creation failures
   - Structure validation 

## Token Deduplication

### Overview
The system provides functionality to remove duplicate tokens before creating string cycles, ensuring efficient storage and preventing redundant token usage.

### Function Interface
```c
int remove_duplicates(uint* arr, int count)
```

#### Parameters
- arr: Array of token indices to deduplicate
- count: Number of elements in array

#### Returns
- New count after removing duplicates
- Original count if no duplicates found or count <= 1

### Process
1. Input Validation
   - Return immediately if count <= 1
   - Preserve array if no duplicates

2. Duplicate Detection
   - Compare each element with previous unique elements
   - Track unique elements found
   - Skip duplicate elements

3. Array Reorganization
   - Move unique elements to front of array
   - Update count to reflect unique elements
   - Maintain element order

### Implementation Details

#### Duplicate Removal
```c
int new_count = 1;  // Start with first element

for (int i = 1; i < count; i++) {
    bool is_duplicate = false;
    
    // Check against previous unique elements
    for (int j = 0; j < new_count; j++) {
        if (arr[i] == arr[j]) {
            is_duplicate = true;
            break;
        }
    }
    
    // Keep unique elements
    if (!is_duplicate) {
        arr[new_count] = arr[i];
        new_count++;
    }
}
```

#### Usage in String Creation
```c
// Remove duplicates before creating cycle
count = remove_duplicates(tokens, count);
optimize_string_cycle(tokens, count);
```

### Benefits
1. Storage Efficiency
   - Eliminates redundant tokens
   - Reduces cycle size
   - Optimizes memory usage

2. Performance
   - Simpler cycle structure
   - Fewer token traversals
   - Cleaner string representation

3. Error Prevention
   - Avoids duplicate token issues
   - Ensures cycle integrity
   - Simplifies optimization

### Notes
1. Array Modification
   - Modifies array in-place
   - Original order preserved for unique elements
   - No additional memory allocation

2. Performance Considerations
   - O(n²) comparison operations
   - Suitable for small token arrays
   - Single pass through array

3. Usage Context
   - Called before cycle optimization
   - Part of string creation process
   - Improves cycle efficiency 

## Cycle Membership

### Overview
The system provides functionality to check whether two vertices belong to the same cycle, which is essential for maintaining cycle integrity and preventing invalid operations.

### Function Interface
```c
bool are_vertices_in_same_cycle(uint node1, ushort channel1, 
                              uint node2, ushort channel2, 
                              ushort axis_number)
```

#### Parameters
- node1: First vertex's node index
- channel1: First vertex's channel index
- node2: Second vertex's node index
- channel2: Second vertex's channel index
- axis_number: Axis to check for cycle membership

#### Returns
- true: Vertices are in the same cycle
- false: Vertices are not in the same cycle or error occurred

### Process
1. Cycle Information Retrieval
   - Get cycle information starting from first vertex
   - Handle case where no cycle exists
   - Manage memory allocation

2. Vertex Search
   - Search for second vertex in cycle
   - Check both node and channel indices
   - Handle cycle traversal

3. Resource Management
   - Free cycle information structure
   - Handle memory cleanup
   - Return result

### Implementation Details

#### Cycle Check
```c
cycleInfo* cycle = get_cycle_info(node1, channel1, axis_number);
if (!cycle) return false;

// Search for second vertex
bool found = false;
for (int i = 0; i < cycle->count; i++) {
    if (cycle->vertices[i] == node2 && 
        cycle->channels[i] == channel2) {
        found = true;
        break;
    }
}
```

#### Usage Example
```c
// Check if vertices are in same cycle
if (are_vertices_in_same_cycle(42, 1, 43, 2, STRING_AXIS)) {
    printf("Vertices are in the same cycle\n");
} else {
    printf("Vertices are not in the same cycle\n");
}
```

### Benefits
1. Validation
   - Prevents invalid cycle operations
   - Ensures cycle integrity
   - Validates vertex relationships

2. Error Prevention
   - Early detection of invalid operations
   - Clear cycle membership checks
   - Robust error handling

3. Cycle Management
   - Supports cycle operations
   - Maintains cycle structure
   - Enables safe modifications

### Notes
1. Performance
   - O(n) cycle traversal
   - Single cycle check
   - Efficient memory usage

2. Error Handling
   - Handles NULL cycle info
   - Validates input parameters
   - Returns false on errors

3. Usage Context
   - Pre-operation validation
   - Cycle modification checks
   - Relationship verification 