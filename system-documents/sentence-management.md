# String Management

## Overview
The system provides functionality to create strings by linking token vertices in a cycle using dedicated channels. Each string is represented as a cycle of tokens connected through axis 2 (string axis).

## String Creation

### Process
1. Token Search
   ```c
   TokenSearchResult *result_first = search_token(current_pos, remaining_len);
   ```
   - Search for matching token sequence
   - Return token node index and matched length
   - Handle search failures

2. Single Token Case
   ```c
   if (remaining_len == result_first->matched_length) {
       tokens[0] = result_first->node_index;
       channels[0] = recycle_or_create_channel(result_first->node_index);
       create_loop(tokens[0], channels[0], 2);
   }
   ```
   - Handle single token strings
   - Create self-loop for single token
   - Reuse or create new channel

3. Multiple Token Processing
   ```c
   while (remaining_len > 0 && count < MAX_STRING_TOKENS) {
       TokenSearchResult *result = search_token(current_pos, remaining_len);
       tokens[count] = result->node_index;
       
       // Handle repeated tokens
       if (tokens[count] == tokens[count-1]) {
           create_channel(tokens[count]);
           channels[count] = channels[count-1] + 1;
       } else {
           channels[count] = recycle_or_create_channel(result->node_index);
       }
   }
   ```
   - Process each token sequentially
   - Handle token repetition
   - Manage channel allocation

### Channel Management
1. Token Repetition
   - Create new channel for repeated tokens
   - Increment channel number sequentially
   - Maintain token sequence integrity

2. Channel Allocation
   - Reuse empty channels when possible
   - Create new channels when needed
   - Track channel assignments

### Implementation Notes
1. Memory Management
   - Fixed token array size (MAX_STRING_TOKENS)
   - Proper result cleanup
   - Channel resource management

2. Error Handling
   - Token search failures
   - Channel creation errors
   - Array bounds checking

3. Performance
   - Efficient channel reuse
   - Minimal memory allocation
   - Sequential processing

### Command Interface
```shell
create-string-str <text>
```

Example:
```shell
> create-string-str "Hello World"
Successfully created string starting at node 5, channel 0
```

## String-Based string Creation

### Process
1. Character Processing
   - Convert each character to token node index
   - Use ASCII value as node index (0-255)
   - Each character maps to existing token node

2. Token Mapping
   - No new node creation needed
   - Direct ASCII to node mapping
   - Uses pre-existing token vertices

3. string Formation
   - Create cycle from mapped tokens
   - Link tokens sequentially
   - Maintain character order

### Command Interface
```shell
create-string-str <text>
```

#### Parameters
- text: ASCII text to form string
- Each character maps to node 0-255
- Minimum 2 characters required

#### Examples
```shell
# Create string from text
> create-string-str Hello
Successfully created string with 5 characters

# Error cases
> create-string-str
Error: Missing arguments
Usage: create-string-str <text>
Example: create-string-str Hello

> create-string-str H
Error: At least 2 characters required for a string

> create-string-str Hello©World
Error: Invalid character (outside ASCII range)
```

### Implementation Details

#### Character Mapping
```c
// Map each character to token node
uint token_node = (unsigned char)args[i];  // ASCII value 0-255
tokens[count++] = token_node;
```

#### string Assembly
```c
// Create cycle using existing token vertices
create_string_cycle(tokens, count);
```

### Notes
1. Token Usage
   - Uses existing vertices 0-255
   - No node creation needed
   - Direct ASCII mapping

2. Memory Management
   - Only token array allocation
   - Minimal memory usage
   - Simple cleanup

3. Error Handling
   - ASCII range validation
   - Character count check
   - Invalid character detection

4. Performance
   - O(n) character processing
   - No node creation overhead
   - Efficient direct mapping

5. Character Encoding
   - Full UTF-8 support
   - Multi-byte character handling
   - Invalid sequence detection
   - Requires UTF-8 terminal configuration:
     ```bash
     # Add to ~/.bashrc or run before using the program
     export LANG=en_US.UTF-8
     export LC_ALL=en_US.UTF-8
     ```

## string Retrieval

### Process
1. Cycle Validation
   - Check for valid string cycle
   - Verify cycle completeness
   - Validate starting point

2. Token Collection
   - Traverse cycle in order
   - Get data from each token
   - Concatenate token data

3. Data Assembly
   - Add spaces between tokens
   - Maintain order from start point
   - Handle buffer management

### Command Interface
```shell
get-string <node_index> <channel_index>
```

#### Parameters
- node_index: Starting node of string cycle
- channel_index: Channel containing string cycle

#### Output Format
The command displays string data in three formats:
1. Raw: Data as stored in the database
2. HEX: Hexadecimal representation of each byte
3. UTF-8: Properly decoded Unicode text with support for:
   - ASCII characters (1 byte)
   - European/Latin characters (2 bytes)
   - CJK characters (3 bytes)
   - Supplementary characters (4 bytes)

#### Examples
```shell
# Get English string
> get-string 42 1
string data starting from node 42, channel 1:
Raw: Hello World
HEX: 48 65 6C 6C 6F 20 57 6F 72 6C 64
UTF-8: Hello World

# Get Korean string
> get-string 43 1
string data starting from node 43, channel 1:
Raw: 안녕하세요
HEX: EC 95 88 EB 85 95 ED 95 98 EC 84 B8 EC 9A 94
UTF-8: 안녕하세요

# Get mixed language string
> get-string 44 1
string data starting from node 44, channel 1:
Raw: Hello 世界
HEX: 48 65 6C 6C 6F 20 E4 B8 96 E7 95 8C
UTF-8: Hello 世界

# Get string with invalid UTF-8
> get-string 45 1
string data starting from node 45, channel 1:
Raw: HelloWorld
HEX: 48 65 6C 6C 6F FF 57 6F 72 6C 64
UTF-8: Hello\xFFWorld
```

### Implementation Details

#### UTF-8 Decoding
```c
// Handle multi-byte UTF-8 characters
if (c < 0x80) {         // ASCII (1 byte)
    printf("%c", c);
    i += 1;
}
else if (c < 0xE0) {    // 2-byte UTF-8
    printf("%c%c", data[i], data[i+1]);
    i += 2;
}
else if (c < 0xF0) {    // 3-byte UTF-8
    printf("%c%c%c", data[i], data[i+1], data[i+2]);
    i += 3;
}
else if (c < 0xF8) {    // 4-byte UTF-8
    printf("%c%c%c%c", data[i], data[i+1], data[i+2], data[i+3]);
    i += 4;
}
```

### Notes
1. Character Encoding
   - Full UTF-8 support
   - Multi-byte character handling
   - Invalid sequence detection

2. Language Support
   - ASCII (English)
   - European languages
   - CJK languages
   - Emoji and symbols

3. Error Handling
   - Incomplete sequences
   - Invalid UTF-8 bytes
   - Proper escape sequences

4. Performance
   - Efficient byte processing
   - Proper string length handling
   - Safe buffer management 

### Special Cases

#### Single Token strings
When the string data exactly matches an existing token, special handling is required:

1. Detection
   - Single token found during tokenization
   - Exact match with existing token data
   - No need for token combination

2. Channel Creation
   - Add new channel to existing token
   - Use for self-referential cycle
   - Preserve token structure

3. Self-Loop Creation
   - Create link to same node
   - Use axis 2 for string structure
   - Same channel for source and destination

#### Implementation Details
```c
// Handle single token case
if (count == 1) {
    // Create new channel for self-loop
    create_channel(token_node);
    ushort new_channel = get_channel_count(node) - 1;
    
    // Create self-loop using axis 2
    create_link(token_node, new_channel, token_node, new_channel, 2);
}
```

### Benefits
1. Storage Efficiency
   - Reuses existing tokens
   - Minimal additional storage
   - Simple cycle structure

2. Data Consistency
   - Maintains token uniqueness
   - Clear relationship representation
   - Efficient data access

3. Implementation Simplicity
   - Straightforward handling
   - Clear cycle structure
   - Easy maintenance 

## string Creation Process

### Overview
The string creation process involves tokenizing input data and creating a cycle of tokens. The process includes token optimization through combination when possible.

### Detailed Process

#### 1. Initial Token Search
```c
TokenSearchResult *result_first = search_token(current_pos, remaining_len);
```
- Searches for longest matching token from input
- Returns token node index and matched length
- Handles initial token discovery

#### 2. Single Token Case
If input matches a single token completely:
1. Create/recycle channel
2. Create self-loop using axis 2
3. Return success

```c
if (remaining_len == result_first->matched_length) {
    int channel_index = recycle_or_create_channel(result_first->node_index);
    create_loop(result_first->node_index, channel_index, 2);
}
```

#### 3. Multi-Token Processing
For inputs requiring multiple tokens:

1. Token Collection
   ```c
   uint tokens[MAX_string_TOKENS];
   ushort channels[MAX_string_TOKENS];
   ```
   - Stores token vertices and channels
   - Tracks token sequence
   - Limited by MAX_string_TOKENS

2. Repeat Token Handling
   ```c
   if (tokens[count] == tokens[count-1]) {
       create_channel(tokens[count]);
       channels[count] = channels[count-1] + 1;
   } else {
       channels[count] = recycle_or_create_channel(result->node_index);
   }
   ```
   - Special handling for repeated tokens
   - Creates new channel explicitly
   - Increments channel index

3. Token Linking
   ```c
   if (count > 0) {
       create_link(tokens[count-1], channels[count-1], 
                  tokens[count], channels[count], 2);
   }
   ```
   - Links consecutive tokens
   - Uses axis 2 for string structure
   - Maintains token sequence

#### 4. Token Optimization
For each token after the first:

1. Channel Count Check
   ```c
   prev_channel_count = get_channel_count(Core[get_node_position(tokens[count-1])]);
   if (prev_channel_count <= 2) {
       need_search = false;
   }
   ```
   - Skip optimization if insufficient channels
   - Prevents unnecessary searches
   - Improves performance

2. Channel Search
   ```c
   for (ushort ch = 1; ch < prev_channel_count; ch++) {
       if (ch == channels[count-1]) continue;
       // Search for matching tokens...
   }
   ```
   - Examines each channel except current
   - Looks for matching token sequences
   - Enables token combination

3. Skip Conditions
   - Empty axis 2 (cleared channels)
   - Current string channel
   - Invalid link configurations

4. Token Combination
   ```c
   if (strcmp(next_token, result->token_data) == 0) {
       int new_node = create_token_node(prev_node, result->node_index);
       create_multi_channels(new_node, 2);
       // Update string structure...
   }
   ```
   - Creates combined token node
   - Allocates necessary channels
   - Updates string links

### Implementation Notes

#### Channel Management
1. Channel Creation
   - Explicit creation for repeat tokens
   - Recycling for new tokens
   - Multiple channels for combined tokens

2. Channel Selection
   - Skip current string channel
   - Skip cleared channels
   - Consider all other channels

#### Token Optimization Strategy
1. Search Criteria
   - Minimum channel count (>2)
   - Valid axis 2 presence
   - Matching token data

2. Combination Benefits
   - Reduced token count
   - Optimized storage
   - Improved traversal

3. Performance Considerations
   - Early search termination
   - Channel count validation
   - Efficient token comparison

### Key Features

#### 1. Token Combination
- Checks previous token's channels for matches
- Creates combined tokens for matching sequences
- Updates cycle structure with combined tokens

#### 2. Channel Management
- Recycles existing channels when possible
- Creates new channels when needed
- Maintains channel count limits

#### 3. Memory Management
- Proper handling of search results
- Clean up of temporary data
- Resource management for token data

### Error Handling

1. Input Validation
   - Empty input check
   - Maximum token limit check
   - Token search failure handling

2. Resource Creation
   - Channel creation failures
   - Token combination errors
   - Cycle creation issues

3. Memory Management
   - Search result cleanup
   - Token data deallocation
   - Error state cleanup

### Usage Examples

```shell
# Basic string creation
> create-string "Hello World"
Successfully created string cycle

# Single token case
> create-string "Hello"
Created self-loop string for single token

# Error cases
> create-string ""
Error: Missing text argument

> create-string "Very long text..."
Error: Maximum token limit exceeded
```