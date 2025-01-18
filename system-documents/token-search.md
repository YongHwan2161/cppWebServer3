# Token Search

## Overview
The token search functionality allows finding the longest matching sequence of tokens for given input data. This enables efficient storage by reusing existing tokens and reducing redundant data storage.

## Process
1. Initial Matching
   - Start with first byte as vertex index
   - Get token data from vertex
   - Check for matches in token search axis

2. Link Traversal
   - Follow links in token search axis (axis 0)
   - Check each linked vertex's token data
   - Find longest matching sequence

3. Result Generation
   - Return matched vertex index
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
Final vertex: 187
Token data: AABB

# Search with existing sequence
> search-token Hello
Search results for: Hello
Matched length: 5 bytes
Final vertex: 111
Token data: Hello
```

## Implementation Details

### Data Structure
```c
typedef struct {
    uint vertex_index;    // Found vertex index
    char* token_data;     // Token data at vertex
    int matched_length;   // Length of matched data
} TokenSearchResult;
```

### Search Process
```c
// Get links from current vertex
uint vertex_position = get_vertex_position(current_vertex);
if (!Core[vertex_position]) break;

// Check if token search axis exists
uint channel_offset = get_channel_offset(Core[vertex_position], 0);
if (!has_axis(Core[vertex_position], 0, TOKEN_SEARCH_AXIS)) {
    break;
}

// Get axis offset and link count
uint axis_offset = get_axis_offset(Core[vertex_position], 0, TOKEN_SEARCH_AXIS);
ushort link_count = *(ushort*)(Core[vertex_position] + channel_offset + axis_offset);

// Check each link's destination
for (int i = 0; i < link_count; i++) {
    uint next_vertex = *(uint*)(Core[vertex_position] + link_offset + (i * 6));
    char* next_token = get_token_data(next_vertex);
    
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
   - Validate vertex position

2. Token Comparison
   - Compare full token with input data
   - Match from start of remaining data
   - Update position on match

3. Debug Information
   - Vertex positions
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
   - Invalid vertex checks
   - NULL token data handling

4. Debug Features
   - Vertex position logging
   - Link count display
   - Token content printing
   - Match length tracking

4. Applications
   - Data deduplication
   - Storage optimization
   - Pattern matching 