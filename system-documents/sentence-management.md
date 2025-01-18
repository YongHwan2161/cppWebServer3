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
The command displays sentence data in two formats:
1. ASCII: Human-readable text format
2. HEX: Hexadecimal representation of each byte

#### Examples
```shell
# Get sentence data
> get-sentence 42 1
Sentence data starting from vertex 42, channel 1:
ASCII: Hello World Example
HEX: 48 65 6C 6C 6F 20 57 6F 72 6C 64 20 45 78 61 6D 70 6C 65

# Error cases
> get-sentence 42
Error: Missing arguments
Usage: get-sentence <vertex_index> <channel_index>

> get-sentence 256 0
Error: Invalid vertex index

> get-sentence 42 1
Error: No valid sentence cycle found
```

### Implementation Details

#### Cycle Traversal
1. Starting Point
   ```c
   cycleInfo* info = get_cycle_info(vertex_index, channel_index, SENTENCE_AXIS);
   ```

2. Token Processing
   ```c
   for (int i = 0; i < info->count; i++) {
       char* token_data = get_token_data(info->vertices[i]);
       // Process token data
   }
   ```

3. Data Assembly
   ```c
   // Add token data to sentence
   strcpy(sentence + sentence_len, token_data);
   sentence_len += token_len;

   // Add space between tokens
   if (i < info->count - 1) {
       sentence[sentence_len++] = ' ';
   }
   ```

### Notes
1. Order Dependence
   - Different start points = different output
   - Cycle order preserved
   - Token sequence matters

2. Memory Management
   - Dynamic buffer allocation
   - Token data cleanup
   - Cycle info cleanup

3. Error Handling
   - Invalid cycles
   - Missing tokens
   - Buffer overflow

4. Performance
   - Linear time traversal
   - Minimal memory usage
   - Efficient string handling 