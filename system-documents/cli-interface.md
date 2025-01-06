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

## Implementation Details

### Command Handler
The command handler processes user input in three steps:
1. Parse command and arguments
2. Validate input values
3. Execute appropriate function

### Error Handling
- Invalid command format
- Out of range values
- System errors
- Resource allocation failures

## Usage Example
```
CGDB Command Line Interface
Type 'help' for available commands

> help
Available commands:
  create-axis <node> <channel> <axis>  Create a new axis
  help                                 Show this help message
  exit                                 Exit the program

> create-axis 0 0 0
Successfully created axis 0 in node 0, channel 0

> exit
``` 