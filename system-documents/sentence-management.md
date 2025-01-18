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

## Implementation Details

### Channel Management
1. New Channel Creation
   ```c
   // Create unique channel for each token
   create_sequential_channel(token_vertex);
   ```

2. Channel Independence
   - Each sentence uses separate channels
   - Prevents interference between sentences
   - Maintains path isolation

### Cycle Creation
1. Link Formation
   ```c
   // Create cycle using new channels
   create_cycle(tokens, channels, count, SENTENCE_AXIS);
   ```

2. Link Properties
   - Sequential token connections
   - Cycle completion
   - Sentence axis usage

### Best Practices
1. Channel Usage
   - Create new channels for each sentence
   - Don't reuse existing channels
   - Maintain path independence

2. Error Handling
   - Validate token indices
   - Check channel creation
   - Verify cycle formation

3. Resource Management
   - Clean up on failures
   - Free temporary arrays
   - Handle memory allocation

### Notes
1. Path Independence
   - Each sentence uses unique channels
   - Prevents cross-sentence interference
   - Maintains data structure integrity

2. Memory Management
   - Dynamic channel allocation
   - Proper cleanup on errors
   - Resource tracking

3. Error Recovery
   - Partial cleanup on failures
   - Clear error messages
   - State consistency 