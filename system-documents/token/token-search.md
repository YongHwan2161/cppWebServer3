# Token Search

## Overview
The token search functionality allows finding the longest matching sequence of tokens for given input data. This enables efficient storage by reusing existing tokens and reducing redundant data storage.

## Process
1. Initial Matching
   - Start with first byte as node index
   - Get token data from node
   - Check for matches in token search axis

2. Link Traversal
   - Follow links in token search axis (axis 0)
   - Check each linked node's token data
   - Find longest matching sequence

3. Result Generation
   - Return matched node index
   - Include matched token data
   - Report matched length

## Command Interface
```shell
search-token <text>
```

### Parameters
- text: Data to search for matching tokens

### Examples
```shell
# Search for token sequence
> search-token AABBCCDD
Search results for: AABBCCDD
Matched length: 4 bytes
Final node: 187
Token data: AABB

# Search with existing sequence
> search-token Hello
Search results for: Hello
Matched length: 5 bytes
Final node: 111
Token data: Hello
```

## Implementation Details

### Data Structure
```c
typedef struct {
    uint node_index;    // Found node index
    char* token_data;     // Token data at node
    int matched_length;   // Length of matched data
} TokenSearchResult;
```

### Search Process
```c
// Get links from current node
uint node_position = get_node_position(current_node);
if (!Core[node_position]) break;

// Check if token search axis exists
uint channel_offset = get_channel_offset(Core[node_position], 0);
if (!has_axis(Core[node_position], 0, TOKEN_SEARCH_AXIS)) {
    break;
}

// Get axis offset and link count
uint axis_offset = get_axis_offset(Core[node_position], 0, TOKEN_SEARCH_AXIS);
ushort link_count = *(ushort*)(Core[node_position] + channel_offset + axis_offset);

// Check each link's destination
for (int i = 0; i < link_count; i++) {
    uint next_node = *(uint*)(Core[node_position] + link_offset + (i * 6));
    char* next_token = get_token_data(next_node);
    
    // Compare full token with input data
    size_t next_len = strlen(next_token);
    if (next_len <= length && memcmp(data, next_token, next_len) == 0) {
        // Found matching token
        matched_pos = next_len;
        // Update current position...
    }
}
```

### Implementation Details

#### Matching Strategy
1. Initial Check
   - Verify token search axis exists
   - Get link count and offsets
   - Validate node position

2. Token Comparison
   - Compare full token with input data
   - Match from start of remaining data
   - Update position on match

3. Debug Information
   - node positions
   - Link counts
   - Token contents
   - Match lengths

### Notes
1. Search Strategy
   - Full token matching
   - Early termination on missing axis
   - Debug output for troubleshooting

2. Performance
   - Axis existence check
   - Direct memory comparison
   - Position tracking

3. Error Handling
   - Missing axis detection
   - Invalid node checks
   - NULL token data handling

4. Debug Features
   - node position logging
   - Link count display
   - Token content printing
   - Match length tracking

4. Applications
   - Data deduplication
   - Storage optimization
   - Pattern matching 

# Token Search Tree Management

## Overview
The token search tree is a hierarchical structure that enables efficient token lookup and reuse. Each node in the tree represents a token, with links through axis 0 (TOKEN_SEARCH_AXIS) connecting to longer token sequences.

## Tree Creation Process

### create_token_search_tree Function
```c
int create_token_search_tree(uint new_node, const char* new_token_data, uint first_node);
```

#### Purpose
- Inserts new token nodes into the search tree structure
- Maintains prefix relationships between tokens
- Enables efficient token lookup and reuse

#### Process
1. Tree Traversal
   - Start from first_node (root)
   - Follow token search axis links
   - Compare token data at each node

2. Path Selection
   - Check if current token is prefix of new token
   - Continue down matching path if found
   - Create new link if no match found

3. Link Creation
   - Add link when no matching prefix exists
   - Connect from last matching node
   - Use token search axis (0)

### Implementation Details

#### Node Traversal
```c
// Get linked nodes through token search axis
uint channel_offset = get_channel_offset(Core[node_position], 0);
uint axis_offset = get_axis_offset(Core[node_position], 0, TOKEN_SEARCH_AXIS);
ushort link_count = *(ushort*)(Core[node_position] + channel_offset + axis_offset);
```

#### Token Comparison
```c
// Compare token data for prefix match
if (strncmp(new_token_data, dest_token, strlen(dest_token)) == 0) {
    // Continue down this path
    current_node = dest_node;
    found_path = true;
}
```

#### Link Creation
```c
// Create new link when no matching path found
create_link(current_node, 0, new_node, 0, TOKEN_SEARCH_AXIS);
```

### Usage Example
```c
// Create new token node
uint new_node = create_token_node(data);

// Insert into search tree starting from root
create_token_search_tree(new_node, token_data, root_node);
```

## Memory Management

### Resource Handling
1. Token Data
   - Free temporary token strings
   - Handle allocation failures
   - Clean up on early returns

2. Node Access
   - Validate node indices
   - Check axis existence
   - Handle missing nodes

### Error Cases
1. Invalid Input
   - NULL token data
   - Invalid node indices
   - Missing search axis

2. Tree Structure
   - Missing linked nodes
   - Corrupted token data
   - Link creation failures

## Performance Considerations

### Optimization
1. Early Termination
   - Stop at first non-matching node
   - Skip invalid nodes
   - Minimize comparisons

2. Memory Usage
   - Free temporary strings promptly
   - Minimize allocations
   - Efficient token storage

3. Search Efficiency
   - Prefix-based traversal
   - Minimal backtracking
   - Optimal tree structure 