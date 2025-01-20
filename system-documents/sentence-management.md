# Sentence Management

## Overview
The system provides functionality to create sentences by linking token vertices in a cycle using dedicated channels. Each sentence is represented as a cycle of tokens connected through axis 2 (sentence axis).

## Sentence Creation

### Process
1. Tokenization
   - Input text is tokenized using search_token
   - Each token maps to existing vertex
   - Tokens found sequentially from input

2. Token Optimization
   - Check for existing token combinations
   - Search through channel links
   - Create combined tokens when possible
   - Reduce sentence length through combination

3. Token Combination
   - Create new vertex for combined tokens
   - Remove old token path from cycle
   - Insert new combined token
   - Update token sequence

4. Cycle Formation
   - Link optimized tokens sequentially
   - Create sentence cycle
   - Use axis 2 for connections
   - Return start vertex and channel

### Command Interface
```shell
create-sentence <text>
```

Parameters:
- text: Input text to create sentence from

Returns:
- Success/error status
- Start vertex and channel for sentence access

Example:
```shell
> create-sentence "Hello World"
Successfully created sentence starting at vertex 5, channel 2
```

### Implementation Details
```c
int handle_create_sentence(char* args, uint* start_vertex, ushort* start_channel);
```

#### Parameters
- args: Input text string
- start_vertex: Pointer to store starting vertex index
- start_channel: Pointer to store starting channel index

#### Return Values
- SUCCESS: Sentence created successfully
- ERROR: Failed to create sentence

#### Process Flow
1. Input Processing
   - Validate input text
   - Initialize token arrays
   - Set initial start vertex/channel

2. Token Processing
   - Search and create tokens
   - Optimize token combinations
   - Track start position

3. Cycle Creation
   - Link tokens in sequence
   - Create final cycle link
   - Return start position

4. Error Handling
   - Return start position even on error
   - Clear error state appropriately
   - Provide detailed error messages

### Benefits
1. Storage Optimization
   - Reduced token count
   - Shared token combinations
   - Efficient data representation

2. Performance
   - Faster sentence traversal
   - Fewer cycle elements
   - Optimized memory usage

3. Data Deduplication
   - Common patterns combined
   - Reduced redundancy
   - Consistent representation

## String-Based Sentence Creation

### Process
1. Character Processing
   - Convert each character to token vertex index
   - Use ASCII value as vertex index (0-255)
   - Each character maps to existing token vertex

2. Token Mapping
   - No new vertex creation needed
   - Direct ASCII to vertex mapping
   - Uses pre-existing token vertices

3. Sentence Formation
   - Create cycle from mapped tokens
   - Link tokens sequentially
   - Maintain character order

### Command Interface
```shell
create-sentence-str <text>
```

#### Parameters
- text: ASCII text to form sentence
- Each character maps to vertex 0-255
- Minimum 2 characters required

#### Examples
```shell
# Create sentence from text
> create-sentence-str Hello
Successfully created sentence with 5 characters

# Error cases
> create-sentence-str
Error: Missing arguments
Usage: create-sentence-str <text>
Example: create-sentence-str Hello

> create-sentence-str H
Error: At least 2 characters required for a sentence

> create-sentence-str Hello©World
Error: Invalid character (outside ASCII range)
```

### Implementation Details

#### Character Mapping
```c
// Map each character to token vertex
uint token_vertex = (unsigned char)args[i];  // ASCII value 0-255
tokens[count++] = token_vertex;
```

#### Sentence Assembly
```c
// Create cycle using existing token vertices
create_sentence_cycle(tokens, count);
```

### Notes
1. Token Usage
   - Uses existing vertices 0-255
   - No vertex creation needed
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
   - No vertex creation overhead
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

## Sentence Retrieval

### Process
1. Cycle Validation
   - Check for valid sentence cycle
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
get-sentence <vertex_index> <channel_index>
```

#### Parameters
- vertex_index: Starting vertex of sentence cycle
- channel_index: Channel containing sentence cycle

#### Output Format
The command displays sentence data in three formats:
1. Raw: Data as stored in the database
2. HEX: Hexadecimal representation of each byte
3. UTF-8: Properly decoded Unicode text with support for:
   - ASCII characters (1 byte)
   - European/Latin characters (2 bytes)
   - CJK characters (3 bytes)
   - Supplementary characters (4 bytes)

#### Examples
```shell
# Get English sentence
> get-sentence 42 1
Sentence data starting from vertex 42, channel 1:
Raw: Hello World
HEX: 48 65 6C 6C 6F 20 57 6F 72 6C 64
UTF-8: Hello World

# Get Korean sentence
> get-sentence 43 1
Sentence data starting from vertex 43, channel 1:
Raw: 안녕하세요
HEX: EC 95 88 EB 85 95 ED 95 98 EC 84 B8 EC 9A 94
UTF-8: 안녕하세요

# Get mixed language sentence
> get-sentence 44 1
Sentence data starting from vertex 44, channel 1:
Raw: Hello 世界
HEX: 48 65 6C 6C 6F 20 E4 B8 96 E7 95 8C
UTF-8: Hello 世界

# Get sentence with invalid UTF-8
> get-sentence 45 1
Sentence data starting from vertex 45, channel 1:
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

#### Single Token Sentences
When the sentence data exactly matches an existing token, special handling is required:

1. Detection
   - Single token found during tokenization
   - Exact match with existing token data
   - No need for token combination

2. Channel Creation
   - Add new channel to existing token
   - Use for self-referential cycle
   - Preserve token structure

3. Self-Loop Creation
   - Create link to same vertex
   - Use axis 2 for sentence structure
   - Same channel for source and destination

#### Implementation Details
```c
// Handle single token case
if (count == 1) {
    // Create new channel for self-loop
    create_channel(token_vertex);
    ushort new_channel = get_channel_count(vertex) - 1;
    
    // Create self-loop using axis 2
    create_link(token_vertex, new_channel, token_vertex, new_channel, 2);
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

## Sentence Creation Process

### Overview
The sentence creation process involves tokenizing input data and creating a cycle of tokens. The process includes token optimization through combination when possible.

### Detailed Process

#### 1. Initial Token Search
```c
TokenSearchResult *result_first = search_token(current_pos, remaining_len);
```
- Searches for longest matching token from input
- Returns token vertex index and matched length
- Handles initial token discovery

#### 2. Single Token Case
If input matches a single token completely:
1. Create/recycle channel
2. Create self-loop using axis 2
3. Return success

```c
if (remaining_len == result_first->matched_length) {
    int channel_index = recycle_or_create_channel(result_first->vertex_index);
    create_loop(result_first->vertex_index, channel_index, 2);
}
```

#### 3. Multi-Token Processing
For inputs requiring multiple tokens:

1. Token Collection
   ```c
   uint tokens[MAX_SENTENCE_TOKENS];
   ushort channels[MAX_SENTENCE_TOKENS];
   ```
   - Stores token vertices and channels
   - Tracks token sequence
   - Limited by MAX_SENTENCE_TOKENS

2. Repeat Token Handling
   ```c
   if (tokens[count] == tokens[count-1]) {
       create_channel(tokens[count]);
       channels[count] = channels[count-1] + 1;
   } else {
       channels[count] = recycle_or_create_channel(result->vertex_index);
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
   - Uses axis 2 for sentence structure
   - Maintains token sequence

#### 4. Token Optimization
For each token after the first:

1. Channel Count Check
   ```c
   prev_channel_count = get_channel_count(Core[get_vertex_position(tokens[count-1])]);
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
   - Current sentence channel
   - Invalid link configurations

4. Token Combination
   ```c
   if (strcmp(next_token, result->token_data) == 0) {
       int new_vertex = create_token_vertex(prev_vertex, result->vertex_index);
       create_multi_channels(new_vertex, 2);
       // Update sentence structure...
   }
   ```
   - Creates combined token vertex
   - Allocates necessary channels
   - Updates sentence links

### Implementation Notes

#### Channel Management
1. Channel Creation
   - Explicit creation for repeat tokens
   - Recycling for new tokens
   - Multiple channels for combined tokens

2. Channel Selection
   - Skip current sentence channel
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
# Basic sentence creation
> create-sentence "Hello World"
Successfully created sentence cycle

# Single token case
> create-sentence "Hello"
Created self-loop sentence for single token

# Error cases
> create-sentence ""
Error: Missing text argument

> create-sentence "Very long text..."
Error: Maximum token limit exceeded
```

### Implementation Notes

1. Performance Considerations
   - Token search optimization
   - Channel recycling
   - Efficient combination detection

2. Memory Usage
   - Fixed-size token array (MAX_SENTENCE_TOKENS)
   - Dynamic token data allocation
   - Proper resource cleanup

3. Limitations
   - Maximum token count (100)
   - Channel count restrictions
   - Token combination depth

[Rest of the document remains the same...] 