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