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