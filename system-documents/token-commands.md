# Token Management Commands

## Overview
The system provides commands for managing and retrieving token data from vertices. Token data is stored in a hierarchical structure using axis 2 links.

## Commands

### Get Token Data
```shell
get-token <vertex_index>
```

#### Purpose
Retrieves and displays the token data stored in a vertex by traversing its hierarchical structure.

#### Parameters
- vertex_index: Index of the token vertex to read (0-255)

#### Process
1. Input Validation
   - Verify vertex index
   - Check vertex exists
   - Validate token structure

2. Data Retrieval
   - Traverse token hierarchy
   - Collect leaf node values
   - Assemble complete data

3. Output Format
   - Display vertex index
   - Show retrieved data
   - Handle special characters

#### Usage Examples
```shell
# Get data from a token vertex
> get-token 42
Token data from vertex 42: Hello, World!

# Error cases
> get-token
Error: Missing arguments
Usage: get-token <vertex_index>
Example: get-token 42

> get-token 256
Error: Invalid vertex index

> get-token abc
Error: Invalid arguments
Usage: get-token <vertex_index>
```

#### Notes
1. Memory Management
   - Automatic cleanup
   - No memory leaks
   - Resource handling

2. Error Handling
   - Invalid vertex indices
   - Missing arguments
   - Malformed token structure

3. Performance
   - Efficient traversal
   - Stack-based implementation
   - Minimal memory usage

## Implementation Details

### Command Handler
```c
int handle_get_token_data(char* args) {
    // Parse vertex index
    unsigned int vertex_index;
    if (sscanf(args, "%u", &vertex_index) != 1) {
        print_argument_error("get-token", "<vertex_index>", false);
        return CMD_ERROR;
    }
    
    // Get and display token data
    char* data = get_token_data(vertex_index);
    if (data) {
        printf("Token data from vertex %u: %s\n", vertex_index, data);
        free(data);
        return CMD_SUCCESS;
    }
    return CMD_ERROR;
}
```

### Error Messages
1. Missing Arguments
   ```
   Error: Missing arguments
   Usage: get-token <vertex_index>
   Example: get-token 42
   ```

2. Invalid Vertex
   ```
   Error: Invalid vertex index
   ```

3. Failed Retrieval
   ```
   Error: Failed to get token data from vertex <index>
   ```

### Best Practices
1. Input Validation
   - Check argument count
   - Validate vertex index
   - Verify vertex exists

2. Memory Management
   - Free returned data
   - Handle allocation failures
   - Clean up on errors

3. User Experience
   - Clear error messages
   - Helpful usage examples
   - Consistent output format 