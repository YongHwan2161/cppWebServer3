# Token Management Commands

## Overview
The system provides commands for managing and retrieving token data from vertices. Token data is stored in a hierarchical structure using axis 2 links.

## Commands

### Create Token
```shell
create-token <first_node> <second_node>
```

#### Purpose
Creates a new token node by combining two existing tokens. The new token's data will be the concatenation of the two input tokens' data.

#### Parameters
- first_node: Index of first token node
- second_node: Index of second token node

#### Process
1. Token Creation
   - Create new node
   - Set up token data structure
   - Link to existing tokens

2. Data Organization
   - Axis 0: Token search links
   - Axis 2: Token data storage
   - Maintains hierarchical structure

3. Link Creation
   - Search link from first token
   - Data links to both tokens
   - Order preservation

#### Usage Examples
```shell
# Create token from two existing tokens
> create-token 42 43
Successfully created token node 44 with data: Hello World

# Error cases
> create-token
Error: Missing arguments
Usage: create-token <first_node> <second_node>
Example: create-token 42 43

> create-token 256 42
Error: Invalid node index

> create-token abc 42
Error: Invalid arguments
Usage: create-token <first_node> <second_node>
```

#### Notes
1. Token Structure
   - New token combines existing tokens
   - Order matters for data combination
   - Hierarchical organization

2. Memory Management
   - Automatic node creation
   - Proper link management
   - Resource cleanup

3. Error Handling
   - Invalid node indices
   - Link creation failures
   - Data validation

## Implementation Details

### Token Creation Process
1. node Creation
   ```c
   create_new_node();  // Create base node
   create_axis(new_node, 0, 2);  // Create data axis
   ```

2. Link Setup
   ```c
   // Search link
   create_link(first_node, 0, new_node, 0, 0);
   
   // Data links
   create_link(new_node, 0, first_node, 0, 2);
   create_link(new_node, 0, second_node, 0, 2);
   ```

3. Data Validation
   ```c
   char* data = get_token_data(new_node);
   // Verify combined token data
   ```

### Best Practices
1. Input Validation
   - Check node indices
   - Verify token existence
   - Validate data structure

2. Link Management
   - Maintain search hierarchy
   - Preserve data order
   - Ensure link consistency

3. Error Recovery
   - Clean up on failures
   - Report specific errors
   - Maintain database integrity 