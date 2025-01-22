# Token Management

## Testing

### Sequential Token Creation Test
The system provides a test to verify proper token creation through string creation:

#### Test Process
1. String Building
   - Starts with 2-character string ("AB")
   - Incrementally adds characters ("ABC", "ABCD", etc.)
   - Tests up to full alphabet sequence

2. string Creation
   - Uses handle_create_string for each string
   - Automatically handles token creation and linking
   - Creates cycle structure for each string

3. Verification
   - Ensures each string creation succeeds
   - Verifies complete token creation process
   - Tests cycle formation functionality

#### Command Interface
```shell
test-sequential-token
```

#### Example Output
```
Testing sequential token creation...
Creating string for: AB
Successfully created string for AB
Creating string for: ABC
Successfully created string for ABC
Creating string for: ABCD
Successfully created string for ABCD
...
Sequential token creation tests completed. Failed tests: 0
```

#### Implementation Details
1. string Creation
   - Uses handle_create_string function
   - Automatically handles token search and creation
   - Creates proper cycle structure

2. Token Management
   - Automatic token splitting and combination
   - Proper cycle formation
   - Efficient token reuse

3. Error Handling
   - string creation failures
   - Invalid string handling
   - Resource management

#### Benefits
1. Integration Testing
   - Tests complete string creation flow
   - Verifies token management system
   - Validates cycle creation

2. Error Detection
   - Early failure identification
   - Clear error reporting
   - System stability verification

3. System Validation
   - End-to-end functionality testing
   - Memory management verification
   - Token reuse optimization

#### Notes
- Each string creation results in a cycle structure
- Tokens are automatically created and combined as needed
- System handles memory management automatically
- Failures are clearly reported and tracked 

### Repeating Pattern Test
The system provides a test to verify proper handling of repeating patterns in string creation:

#### Test Process
1. Pattern Generation
   - Tests patterns of length 1 to 10
   - Uses sequential letters (A, B, C, etc.)
   - Creates patterns like "A", "AB", "ABC", etc.

2. Repetition Testing
   - Repeats each pattern 2 to 10 times
   - Creates test strings like "AA", "AAA", "ABABAB", etc.
   - Tests all combinations of pattern length and repetition count

3. string Creation
   - Creates string for each test string
   - Verifies successful creation
   - Tracks any failures

#### Command Interface
```shell
test-repeating-string
```

#### Example Output
```
Testing repeating string creation...

Testing patterns of length 1
Creating string for: AA (pattern 'A' repeated 2 times)
Successfully created string
Creating string for: AAA (pattern 'A' repeated 3 times)
Successfully created string
...

Testing patterns of length 2
Creating string for: ABAB (pattern 'AB' repeated 2 times)
Successfully created string
...
```

#### Implementation Details
1. Pattern Generation
   - Sequential letter patterns
   - Maximum pattern length: 10
   - Null-terminated strings

2. Repetition Handling
   - Safe string concatenation
   - Maximum total length: 100
   - Buffer overflow prevention

3. Error Tracking
   - Per-pattern failure counting
   - Detailed error reporting
   - Success/failure summary

#### Benefits
1. Edge Case Testing
   - Repeating pattern handling
   - Memory management verification
   - System stability testing

2. Comprehensive Coverage
   - Multiple pattern lengths
   - Various repetition counts
   - All combinations tested

3. Error Detection
   - Pattern-specific failures
   - Creation process validation
   - Resource management verification

#### Notes
- Tests 90 different combinations
- Verifies memory handling
- Ensures stable string creation
- Reports specific failure cases 

## Token Integration

### Overview
The token integration process combines multiple tokens into a single token when possible, optimizing storage and improving efficiency by identifying and merging tokens that share the same next vertex in their string cycles.

### Process
1. Channel Validation
   - Verify node has multiple channels (>1)
   - Skip processing if insufficient channels
   - Return SUCCESS for single-channel nodes

2. Channel Analysis
   ```c
   for (int i = 1; i < channel_count; i++) {
       // Skip invalid channels or string axis
       if (get_axis_count(node_index, i) == 0 || 
           has_axis(node_index, i, string_AXIS)) {
           continue;
       }
       // Process channel...
   }
   ```

3. Token Matching
   - Get next vertex for current channel
   - Compare with next vertices of subsequent channels
   - Identify matching token sequences
   ```c
   Vertex next_vertex = get_next_vertex(node_index, i, string_AXIS);
   // Compare with other channels...
   if (next_vertex.node == next_vertex2.node) {
       // Found matching tokens...
   }
   ```

4. Token Combination
   - Create new combined token node
   - Set up channels for new node
   - Update cycle structure
   ```c
   uint new_node = create_token_node(next_vertex.node, next_vertex2.node);
   create_multi_channels(new_node, 2);
   ```

### Cycle Management

#### Short Cycles (2 vertices)
```c
if (existing_cycle && existing_cycle->count == 2) {
    clear_cycle(existing_cycle);
    create_loop(new_node, 1, 2);
}
```

#### Longer Cycles
```c
else {
    delete_path_from_cycle(node_index, i, 2, 2);
    uint new_path[1] = {new_node};
    ushort new_channels[1] = {1};
    insert_path_into_cycle(node_index, i,
                          new_path, new_channels, 1, 2);
}
```

### Helper Functions

#### Get Next Vertex
```c
Vertex get_next_vertex(unsigned int node_index, 
                      unsigned short channel, 
                      unsigned short axis_number);
```
Returns the next vertex (node and channel) in the specified axis.

### Implementation Notes

1. Channel Processing
   - Skip channels with no axes
   - Skip channels with string axis
   - Process each valid channel pair

2. Token Creation
   - Create new node only once per match
   - Set up proper channel structure
   - Handle cycle modifications

3. Cycle Updates
   - Special handling for 2-vertex cycles
   - Path deletion and insertion for longer cycles
   - Proper cleanup of existing cycles

### Error Handling
- Return SUCCESS for single-channel nodes
- Return ERROR on channel creation failure
- Clean up cycle info after use

### Memory Management
- Free cycle info structures
- Create new nodes as needed
- Clean up temporary data

### Usage Example
```c
// Integrate tokens in node 5
int result = integrate_token_data(5);
if (result == SUCCESS) {
    printf("Successfully integrated tokens\n");
} else {
    printf("Token integration failed\n");
}
```

### Notes
1. Performance Considerations
   - Skip invalid channels early
   - Optimize token comparison
   - Minimize memory operations

2. Error Handling
   - Channel count validation
   - Axis existence checks
   - Token data verification

3. Integration Rules
   - Only combine valid token sequences
   - Maintain string integrity
   - Preserve token order

## Command Interface

### Integrate Tokens Command
The system provides a command-line interface for token integration:

```shell
integrate-tokens <node_index>
```

#### Usage
1. Basic Integration
   ```shell
   > integrate-tokens 5
   Successfully integrated tokens in node 5
   ```

2. Error Handling
   ```shell
   > integrate-tokens
   Error: Missing arguments
   Usage: integrate-tokens <node_index>
   ```

#### Implementation
```c
int handle_integrate_tokens(char* args) {
    int node_index;
    if (sscanf(args, "%d", &node_index) != 1) {
        print_argument_error("integrate-tokens", "<node_index>", false);
        return CMD_ERROR;
    }
    return integrate_token_data(node_index) == SUCCESS ? 
           CMD_SUCCESS : CMD_ERROR;
}
```

#### Benefits
1. Direct Access
   - Simple command interface
   - Clear success/failure feedback
   - Immediate integration results

2. Error Protection
   - Input validation
   - Node existence check
   - Clear error messages

3. Integration Control
   - Per-node integration
   - Manual optimization
   - Selective processing

[Rest of the document remains the same...] 