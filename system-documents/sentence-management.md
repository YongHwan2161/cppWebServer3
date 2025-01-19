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

### Command Interface
```shell
create-sentence <text>
```

#### Parameters
- text: Text to be tokenized and formed into sentence
- System automatically finds matching tokens
- Creates cycle from found token vertices

#### Examples
```shell
# Create sentence from text
> create-sentence "Hello World"
Tokenizing: Found 2 tokens
- Token 1: vertex 72 ("Hello")
- Token 2: vertex 87 ("World")
Successfully created sentence cycle with 2 tokens

# Create sentence with longer text
> create-sentence "This is a test"
Tokenizing: Found 4 tokens
- Token 1: vertex 84 ("This")
- Token 2: vertex 32 ("is")
- Token 3: vertex 97 ("a")
- Token 4: vertex 116 ("test")
Successfully created sentence cycle with 4 tokens

# Error cases
> create-sentence
Error: Missing arguments
Usage: create-sentence <text>

> create-sentence "未知の文字列"
Error: Failed to tokenize at position 0: '未知の文字列'
```

### Implementation Details

#### Token Optimization Process
```c
// For each token
for each new_token in input:
    // Check previous token's channels
    for each channel in prev_token:
        // Look for matching next tokens
        if (next_token matches new_token):
            // Create combined token
            new_vertex = create_token_vertex(prev_token, new_token)
            
            // Update cycle
            delete_path(old_tokens)
            insert_path(new_vertex)
```

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

### handle_create_sentence Implementation
```c
int handle_create_sentence(char* args) {
    // Input validation
    if (!args || !*args) {
        print_argument_error("create-sentence", "<text>", false);
        return ERROR;
    }

    uint tokens[MAX_SENTENCE_TOKENS];
    ushort channels[MAX_SENTENCE_TOKENS];
    int count = 0;
    const char* current_pos = args;
    size_t remaining_len = strlen(args);
```

### Key Components

#### 1. Token Search and Channel Creation
```c
TokenSearchResult *result_first = search_token(current_pos, remaining_len);
if (!result_first) {
    printf("Error: Failed to search token\n");
    return ERROR; 
}

// Handle single token case
if (remaining_len == (size_t)result_first->matched_length) {
    int channel_index = recycle_or_create_channel(result_first->vertex_index);
    if (channel_index == CHANNEL_ERROR) {
        printf("Error: Failed to create channel for vertex %u\n", result_first->vertex_index);
        free_search_result(result_first);
        return ERROR;
    }

    // Create self-loop for single token
    if (create_loop(result_first->vertex_index, channel_index, 2) != LINK_SUCCESS) {
        printf("Error: Failed to create loop for vertex %u\n", result_first->vertex_index);
        free_search_result(result_first);
        return ERROR;
    }
    free_search_result(result_first);
    return SUCCESS;
}
```

#### 2. Token Combination Process
```c
bool need_search = true;
while (remaining_len > 0 && count < MAX_SENTENCE_TOKENS) {
    TokenSearchResult *result = search_token(current_pos, remaining_len);
    if (!result) break;
    
    tokens[count] = result->vertex_index;
    channels[count] = recycle_or_create_channel(result->vertex_index);
    
    // Check for possible token combinations
    if (count > 0 && need_search) {
        uint prev_vertex = tokens[count-1];
        // Check each channel for matching next token
        for (ushort ch = 1; ch < prev_channel_count; ch++) {
            uint next_vertex;
            ushort next_channel;
            if (get_link(prev_vertex, ch, 2, 0, &next_vertex, &next_channel) != LINK_SUCCESS) 
                continue;

            // Compare next vertex's token data with current token
            char* next_token = get_token_data(next_vertex);
            if (!next_token) continue;

            if (strcmp(next_token, result->token_data) == 0) {
                // Create combined token
                int new_vertex = create_token_vertex(prev_vertex, result->vertex_index);
                // Update cycle with new combined token
                // ...
            }
            free(next_token);
        }
    }
    // Update position and continue search
    current_pos += result->matched_length;
    remaining_len -= result->matched_length;
    count++;
    free_search_result(result);
}
```

### Process Flow

1. Initial Setup
   - Validate input arguments
   - Initialize token and channel arrays
   - Set up position tracking

2. Single Token Handling
   - Check if input matches single token
   - Create dedicated channel
   - Create self-loop using axis 2

3. Multi-Token Processing
   - Search for tokens sequentially
   - Create/recycle channels for each token
   - Track token and channel information

4. Token Optimization
   - Check for possible token combinations
   - Compare with existing token patterns
   - Create combined tokens when possible

5. Cycle Creation
   - Verify minimum token count (2)
   - Create sentence cycle using tokens
   - Use axis 2 for sentence structure

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