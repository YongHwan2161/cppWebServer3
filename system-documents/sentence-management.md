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