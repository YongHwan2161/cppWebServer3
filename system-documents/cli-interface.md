# Command Line Interface

## Overview
The CGDB system provides a command-line interface for interacting with the database. Users can create axes, manage nodes, and perform other operations through simple text commands.

## Command Structure

### Create Axis
```
create-axis <node_index> <channel_index> <axis_number>
```
- node_index: Target node (0-255)
- channel_index: Target channel in the node
- axis_number: Type of axis to create
  - 0: Forward link
  - 1: Backward link
  - 3: Time axis

Example:
```
> create-axis 0 0 0    # Create forward link axis in node 0, channel 0
> create-axis 1 0 1    # Create backward link axis in node 1, channel 0
> create-axis 0 1 3    # Create time axis in node 0, channel 1
```

### Check Specific Axis
```
check-axis <node_index> <channel_index> <axis_number>
```
Checks if a specific axis exists in the given node and channel.

Example:
```
> check-axis 0 0 0
Axis 0 exists in node 0, channel 0

> check-axis 0 0 2
Axis 2 does not exist in node 0, channel 0
```

### List All Axes
```
list-axes <node_index> <channel_index>
```
Shows all axes present in the specified node and channel.

Example:
```
> list-axes 0 0
Axes in node 0, channel 0:
Total axes: 3

Axis numbers:
- Axis 0 (Forward link)
- Axis 1 (Backward link)
- Axis 3 (Time axis)
- Axis 4
```

Error handling:
```
> list-axes
Error: Missing arguments
Usage: list-axes <node_index> <channel_index>
Example: list-axes 0 0

> list-axes 0
Error: Invalid arguments
Usage: list-axes <node_index> <channel_index>
Example: list-axes 0 0
```

The command will:
1. Show total number of axes
2. List all axis numbers in ascending order
3. Include labels for known axis types (Forward link, Backward link, Time axis)
4. Display custom axis numbers without labels

### Help Command
```
help
```
Displays available commands and their usage.

### Exit Command
```
exit
```
Exits the program, saving all changes.

### Create Link
```
create-link <source_node> <source_ch> <dest_node> <dest_ch> <axis_number>
```
Creates a link between two channels using the specified axis.

Parameters:
- source_node: Source node index (0-255)
- source_ch: Source channel index
- dest_node: Destination node index (0-255)
- dest_ch: Destination channel index
- axis_number: Axis to use for the link
  - 0: Forward link
  - 1: Backward link
  - 3: Time axis

Example:
```
> create-link 0 0 1 0 0
Created link from node 0 channel 0 to node 1 channel 0 using axis 0

> create-link 1 0 0 0 1
Created link from node 1 channel 0 to node 0 channel 0 using axis 1
```

Error handling:
```
> create-link
Error: Missing arguments
Usage: create-link <source_node> <source_ch> <dest_node> <dest_ch> <axis_number>
Example: create-link 0 0 1 0 0

> create-link 0 0
Error: Invalid arguments
Usage: create-link <source_node> <source_ch> <dest_node> <dest_ch> <axis_number>
Example: create-link 0 0 1 0 0
```

### Print Node Data
```
print-node <node_index>
```
Displays detailed node information and its data in hexadecimal format with ASCII representation.

Parameters:
- node_index: Target node (0-255)

Output includes:
1. Node Information
   - Size in bytes
   - Position in Core array
   - File offset in data.bin
   - Load status in memory

2. Memory Contents
   - Hexadecimal view of node data
   - ASCII representation
   - Memory offset for each line

Example:
```
> print-node 0
Node 0 Information:
Size: 32 bytes
Core Position: 0
File Offset: 0x00000000
Load Status: Loaded

Memory Contents:
Offset    00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F    ASCII
--------  -----------------------------------------------    ----------------
00000000  04 00 01 00 08 00 00 00 02 00 00 00 00 00 00 00    ................
00000010  01 00 03 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
```

The output shows:
1. Node metadata
   - Memory allocation size
   - Location in Core array
   - Position in data.bin file
   - Current memory status

2. Memory layout
   - Memory offset (in hexadecimal)
   - Hexadecimal values of each byte
   - ASCII representation of bytes (if printable)

Error handling:
```
> print-node
Error: Missing arguments
Usage: print-node <node_index>
Example: print-node 0

> print-node 256
Error: Node index must be between 0 and 255
```

### Print Free Space Information
```
print-free-space
```
Displays detailed information about the free space management system.

Output includes:
1. Total number of free blocks
2. Number of free node indices
3. List of free blocks with their sizes and offsets
4. List of available node indices

Example:
```
> print-free-space
Free Space Information:
Total free blocks: 2
Free node indices: 1

Free Blocks:
Size (bytes)    Offset
------------    ------
16              0x00001000
32              0x00002000

Free Node Indices:
5
```

The output shows:
1. Summary of free space status
2. Detailed list of available blocks
3. List of node indices available for reuse

Error handling:
```
> print-free-space something
Error: Invalid arguments
Usage: print-free-space
Example: print-free-space
```

### Delete Link
```
delete-link <source_node> <source_ch> <dest_node> <dest_ch> <axis_number>
```
Deletes an existing link between two nodes.

Parameters:
- source_node: Source node index (0-255)
- source_ch: Source channel index
- dest_node: Destination node index (0-255)
- dest_ch: Destination channel index
- axis_number: Axis number for the link

Example:
```
> delete-link 0 0 1 0 0
Successfully deleted link from node 0 channel 0 to node 1 channel 0 using axis 0
```

Error handling:
```
> delete-link
Error: Missing arguments
Usage: delete-link <source_node> <source_ch> <dest_node> <dest_ch> <axis_number>
Example: delete-link 0 0 1 0 0

> delete-link 256 0 1 0 0
Error: Node indices must be between 0 and 255

> delete-link 0 0 1 0 0
Error: Link not found
```

### Test Commands

#### Run All Tests
```
run-tests
```
Executes all available test cases in the system.

Parameters: None

Example:
```
> run-tests
Running all tests...
Testing axis creation...
✓ Successfully created forward axis
...
All tests completed. Total failed tests: 0
```

#### Test Resize Functionality
```
test-resize
```
Runs specific tests for resize_node_space functionality.

Parameters: None

Example:
```
> test-resize
Testing resize_node_space functionality...
✓ Correct number of initial free blocks
...
Resize node space tests completed: 0 failed
```

Error handling:
```
> test-resize something
Error: Invalid arguments
Usage: test-resize
Example: test-resize
```

#### Test Axis Creation/Deletion
```
test-axis-create-delete <node> <channel> <max_axis>
```
Tests complete cycle of axis creation and deletion.

Parameters:
- node: Node index (0-255)
- channel: Channel index
- max_axis: Maximum axis number to create

Example:
```
> test-axis-create-delete 0 0 5
Testing axis creation and deletion (max axis: 5)...
Creating axes 0 to 5...
Verifying created axes...
Deleting all axes...
Verifying axis deletion...
Axis creation/deletion tests completed: 0 failed
```

Error handling:
```
> test-axis-create-delete
Error: Missing arguments
Usage: test-axis-create-delete <node_index> <channel_index> <max_axis>
Example: test-axis-create-delete 0 0 5

> test-axis-create-delete 256 0 5
Error: Node index must be between 0 and 255

> test-axis-create-delete 0 0 -1
Error: Maximum axis number must be non-negative
```

#### Test Free Block Offsets
```
test-free-offsets
```
Verifies that all free blocks have unique and non-overlapping offsets.

Parameters: None

Example:
```
> test-free-offsets
Testing free block offset uniqueness...
Checking 3 free blocks for offset conflicts...
✓ All free block offsets are unique and non-overlapping
Free block offset tests completed: 0 failed
```

Error handling:
```
> test-free-offsets extra_arg
Error: Invalid arguments
Usage: test-free-offsets
Example: test-free-offsets
```

## Error Handling

### Missing Arguments
All commands that require arguments will:
1. Check if arguments are provided
2. Display usage information if arguments are missing
3. Show an example of correct usage

### Invalid Arguments
When arguments are invalid:
1. Clear error message is displayed
2. Usage information is shown
3. Example of correct usage is provided

### Command Results
Each command returns one of these status codes:
- CMD_SUCCESS (0): Command executed successfully
- CMD_ERROR (-1): Error occurred during execution
- CMD_EXIT (1): User requested program exit

## Usage Example
```
CGDB Command Line Interface
Type 'help' for available commands

> help
Available commands:
  create-axis <node> <channel> <axis>  Create a new axis
  check-axis <node> <channel> <axis>   Check if specific axis exists
  list-axes <node> <channel>           List all axes in channel
  help                                 Show this help message
  exit                                 Exit the program

> list-axes
Error: Missing arguments
Usage: list-axes <node_index> <channel_index>
Example: list-axes 0 0

> list-axes 0 0
Axes in node 0, channel 0:
Total axes: 1
Forward link (0): Present
Backward link (1): Not present
Time axis (3): Not present

All axis numbers:
- Axis 0

> exit
```

## Implementation Details

### Error Handling Utilities

#### Common Functions
```c
void print_command_usage(const char* command, const char* args_desc);
void print_argument_error(const char* command, const char* args_desc, bool is_missing);
```

These utility functions provide consistent error handling across all commands:

1. `print_command_usage`: Displays standard usage format for commands
   - command: The command name (e.g., "create-axis")
   - args_desc: Description of required arguments

2. `print_argument_error`: Handles missing or invalid arguments
   - command: The command name
   - args_desc: Description of required arguments
   - is_missing: true for missing arguments, false for invalid ones

### Error Messages Format
All error messages follow a consistent format:
```
Error: [Missing|Invalid] arguments
Usage: <command> <arguments>
Example: <command> <example_values>
```

### Command Implementation
Each command handler follows a standard structure:
1. Check for missing arguments
2. Parse provided arguments
3. Validate input values
4. Execute command logic
5. Return appropriate status code

Example implementation:
```c
int handle_command(char* args) {
    // Check for missing arguments
    if (!args) {
        print_argument_error("command", "<arg1> <arg2>", true);
        return CMD_ERROR;
    }
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d", &arg1, &arg2);
    if (parsed != 2) {
        print_argument_error("command", "<arg1> <arg2>", false);
        return CMD_ERROR;
    }
    
    // Command logic...
}
``` 