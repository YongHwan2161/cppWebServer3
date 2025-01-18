# Sentence Management

## Overview
The system provides functionality to create sentences by linking token vertices in a cycle using dedicated channels. Each sentence is represented as a cycle of tokens connected through axis 2 (sentence axis).

## Sentence Creation

### Process
1. Channel Creation
   - New channel created for each token
   - Prevents path overlap with existing sentences
   - Maintains sentence independence

2. Cycle Formation
   - Tokens linked sequentially
   - Last token links to first
   - Uses sentence axis (2)

### Command Interface
```shell
create-sentence <token1> <token2> [token3...]
```

#### Parameters
- token1, token2, etc: Token vertex indices to form sentence
- Minimum 2 tokens required
- Maximum 100 tokens per sentence

#### Examples
```shell
# Create sentence from 3 tokens
> create-sentence 42 43 44
Successfully created sentence cycle with 3 tokens

# Error cases
> create-sentence 42
Error: At least 2 tokens required for a sentence

> create-sentence 256 42
Error: Invalid token vertex index
```

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