# Command Line Interface

## Overview
The CGDB system provides a command-line interface for interacting with the database. Users can create axes, manage vertices, and perform other operations through simple text commands.

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

### Print node Data
```
print-node <node_index>
```
Displays detailed node information including structure and memory contents.

Parameters:
- node_index: Target node (0-255)

Output includes:
1. node Metadata
   - Size in bytes
   - Position in Core array
   - File offset in data.bin
   - Load status in memory
   - Channel count

2. Channel Information (for each channel)
   - Channel offset
   - Axis count
   - For each axis:
     - Axis number and type
     - Axis offset
     - Link count
     - Link details (destination node and channel)

3. Memory Contents
   - Hexadecimal view of node data
   - ASCII representation
   - Memory offset for each line

Example:
```
> print-node 0
node 0 Information:
Size: 32 bytes
Core Position: 0
File Offset: 0x00000000
Load Status: Loaded

Channel Count: 1

Channel 0 (offset: 0x0008):
  Axis Count: 2
  Axis 0 (Forward link) (offset: 0x0010):
    Link Count: 1
    Link 0: node 1, Channel 0
  Axis 1 (Backward link) (offset: 0x0018):
    Link Count: 0

Memory Contents:
Offset    00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F    ASCII
--------  -----------------------------------------------    ----------------
00000000  04 00 01 00 08 00 00 00 02 00 00 00 00 00 00 00    ................
00000010  01 00 03 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
```

The output shows:
1. Basic node information
   - Memory allocation size
   - Location in Core array
   - Position in data.bin file
   - Current memory status

2. Detailed structure
   - Channel count and offsets
   - Axis information for each channel
   - Link details for each axis

3. Raw memory layout
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
Error: node index must be between 0 and 255
```

### Memory Management Commands

#### Print Free Space
```
print-free-space
```
Displays information about available free space in the data file.

Parameters: None

Example output:
```
Free Space Information:
Total free blocks: 2

Free Blocks:
Size (bytes)    Offset
------------    ------
16              0x00001000
32              0x00002000
```

Error handling:
```shell
> print-free-space extra_args
Error: Invalid arguments
Usage: print-free-space
```

Purpose:
- Monitor available memory blocks
- Track memory fragmentation
- Debug memory allocation issues
- Verify space reclamation

Display Format:
1. Summary Information
   - Total number of free blocks
   - Block count and sizes

2. Block Details
   - Size in bytes
   - File offset in hexadecimal
   - Sorted by offset

### Delete Link
```
delete-link <source_node> <source_ch> <dest_node> <dest_ch> <axis_number>
```
Deletes an existing link between two vertices.

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
Error: node indices must be between 0 and 255

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
- node: node index (0-255)
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
Error: node index must be between 0 and 255

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

### Test Multiple Link Creation
```
test-multiple-link <node_index> <channel_index> <axis_number>
```
Creates and verifies 100 random links for testing purposes.

Parameters:
- node_index: Source node (0-255)
- channel_index: Source channel
- axis_number: Axis to use for links

Example:
```
> test-multiple-link 0 0 0
Testing multiple link creation...
Source: node 0, Channel 0, Axis 0
Link creation test completed: 0 failures

> test-multiple-link 0 0 1
Testing multiple link creation...
Source: node 0, Channel 0, Axis 1
Link creation test completed: 2 failures
Failed to create link 45
Link count mismatch after link 67
```

Error handling:
```
> test-multiple-link
Error: Missing arguments
Usage: test-multiple-link <node_index> <channel_index> <axis_number>
Example: test-multiple-link 0 0 0

> test-multiple-link 256 0 0
Error: node index must be between 0 and 255
```

The command will:
1. Create 100 random links
2. Verify link count after each creation
3. Verify link data correctness
4. Report any failures encountered

### Create Channel
```
create-channel <node_index>
```
Creates a new channel in the specified node.

Parameters:
- node_index: Target node (0-255)

Example:
```
> create-channel 0
Successfully created new channel in node 0

> create-channel 1
Successfully created new channel in node 1
```

Error handling:
```
> create-channel
Error: Missing arguments
Usage: create-channel <node_index>
Example: create-channel 0

> create-channel 256
Error: node index must be between 0 and 255
```

The command will:
1. Validate the node index
2. Create a new channel sequentially
3. Initialize the channel with 0 axes
4. Report success or failure

### Test Commands

#### Multi-Channel Link Test
```
test-multi-channel-links <node_index>
```

Tests link creation and deletion across multiple channels in a node.

Parameters:
- node_index: Target node index (0-255)

Process:
1. Creates a second channel in the target node
2. Creates 100 links in each channel
3. Deletes all links in reverse order
4. Verifies data consistency throughout

Example:
```shell
> test-multi-channel-links 0
Testing link creation/deletion across multiple channels...
Target node: 0
Creating 100 links in each channel...
Deleting all links...
Multi-channel link test completed: 0 failures
```

Error Cases:
```shell
> test-multi-channel-links
Error: Missing arguments
Usage: test-multi-channel-links <node_index>
Example: test-multi-channel-links 0

> test-multi-channel-links 256
Error: node index must be between 0 and 255
```

#### Test Results
The command reports:
1. Channel creation status
2. Link creation progress
3. Link deletion progress
4. Final test results with failure count

##### Success Output
```
Testing link creation/deletion across multiple channels...
Target node: 0
Creating 100 links in each channel...
Deleting all links...
Multi-channel link test completed: 0 failures
```

##### Failure Output
```
Testing link creation/deletion across multiple channels...
Target node: 0
Failed to create channel 1
```

### Channel Creation Test
```
test-channel-creation <node_index>
```

Tests sequential channel creation in a node.

Parameters:
- node_index: Target node index (0-255)

Process:
1. Creates channels sequentially
2. Verifies each channel's structure
3. Checks memory management
4. Reports success or failures

Example:
```shell
> test-channel-creation 0
Testing sequential channel creation...
Target node: 0
Creating channels sequentially...
Final channel count: 10
Channel creation test completed successfully
```

Error Cases:
```shell
> test-channel-creation
Error: Missing arguments
Usage: test-channel-creation <node_index>
Example: test-channel-creation 0

> test-channel-creation 256
Error: node index must be between 0 and 255
```

### Channel Management Commands

#### Clear Channel
```
clear-channel <node_index> <channel_index>
```

Clears all data in a specified channel, resetting it to initial state.

Parameters:
- node_index: Target node (0-255)
- channel_index: Target channel in the node

Process:
1. Removes all axes and links
2. Resets channel to empty state
3. Updates node size and offsets
4. Preserves channel structure

Example:
```shell
> clear-channel 0 1
Successfully cleared channel 1 in node 0
```

Error Cases:
```shell
> clear-channel
Error: Missing arguments
Usage: clear-channel <node_index> <channel_index>
Example: clear-channel 0 1

> clear-channel 256 0
Error: node index must be between 0 and 255

> clear-channel 0 1
Failed to clear channel 1 in node 0
```

The command will:
1. Validate input parameters
2. Check node and channel existence
3. Clear all channel data
4. Update file storage
5. Report operation result

### Get node Position
```
get-node-position <node_index>
```
Shows the actual position of a node in the Core array and its memory address.

Parameters:
- node_index: Target node (0-255)

Example:
```
> get-node-position 0
node 0 is at Core position 0
Memory address: 0x7f8b4c003a00

> get-node-position 5
node 5 is at Core position 3
Memory address: 0x7f8b4c003c80
```

Error handling:
```
> get-node-position
Error: Missing arguments
Usage: get-node-position <node_index>
Example: get-node-position 0

> get-node-position 256
Error: Invalid node index 256

> get-node-position 5
Error: node 5 is not loaded in memory
```

#### Purpose
- Core 배열에서 노드의 실제 위치 확인
- 메모리 주소 표시
- 노드 로드 상태 검증

#### Notes
1. Core Position
   - 노드 인덱스와 다를 수 있음
   - 메모리 관리 상태 반영
   - 동적으로 변경될 수 있음

2. Memory Address
   - 실제 메모리상의 위치
   - 디버깅에 유용
   - 포인터 추적 가능

### Memory Management Commands

#### Unload node
```
unload-node <node_index>
```
Unloads a node from memory. The node data remains in the binary file and can be reloaded when needed.

Parameters:
- node_index: Target node (0-255)

Example:
```
> unload-node 5
Successfully unloaded node 5 from memory
```

Error handling:
```
> unload-node
Error: Missing arguments
Usage: unload-node <node_index>
Example: unload-node 0

> unload-node 256
Error: node index must be between 0 and 255

> unload-node 5
Error: node 5 is not loaded in memory
```

#### Purpose
- 메모리에서 노드 데이터 해제
- 메모리 사용량 최적화
- 필요시 재로드 가능

#### Notes
1. Data Persistence
   - 파일의 데이터는 유지됨
   - 메모리만 해제
   - 필요시 자동 재로드

2. Memory Management
   - CoreMap 상태 업데이트
   - Core 배열 포인터 정리
   - 메모리 누수 방지

#### Load node
```
load-node <node_index>
```
Loads a node from the binary file into memory.

Parameters:
- node_index: Target node (0-255)

Example:
```
> load-node 5
Successfully loaded node 5 to Core position 3
```

Error handling:
```
> load-node
Error: Missing arguments
Usage: load-node <node_index>
Example: load-node 0

> load-node 256
Error: node index must be between 0 and 255

> load-node 5
node 5 is already loaded at Core position 3
```

#### Purpose
- 파일에서 노드 데이터 로드
- Core 배열에 할당
- CoreMap 상태 업데이트

#### Notes
1. Memory Management
   - CoreSize 확인
   - 필요시 다른 노드 언로드
   - 메모리 효율적 사용

2. Data Loading
   - 파일에서 데이터 읽기
   - 메모리 할당
   - 상태 정보 갱신

### Print CoreMap Status
```
print-coremap [node_index]
```
Displays the current status of CoreMap. If a node index is provided, shows information for that specific node. Otherwise, shows all loaded vertices.

Parameters:
- node_index: (optional) Target node (0-255)

Example:
```
> print-coremap
CoreMap Status:
Total Loaded vertices: 3

node     Core Position   Is Loaded      File Offset
--------------------------------------------------------
0        0              Yes            0x00000000
5        1              Yes            0x00000050
10       2              Yes            0x000000A0

> print-coremap 5
CoreMap Status for node 5:
node     Core Position   Is Loaded      File Offset
--------------------------------------------------------
5        1              Yes            0x00000050
```

Error handling:
```
> print-coremap 256
Error: node index must be between 0 and 255

> print-coremap abc
Error: Invalid arguments
Usage: print-coremap <node_index>
Example: print-coremap 0
```

#### Purpose
- CoreMap 상태 모니터링
- 특정 노드 정보 조회
- 메모리 사용 현황 파악

#### Notes
1. Display Information
   - node index
   - Position in Core array
   - Load status
   - File offset in data.bin

2. View Options
   - No argument: Show all loaded vertices
   - node index: Show specific node info

### Check Core Size
```
check-core-size
```
Shows current Core memory usage statistics and capacity information.

Example:
```
> check-core-size
Core Memory Status:
Current Core Size: 3
Maximum Core Size: 16
Available Slots: 13
Memory Utilization: 18.8%
```

#### Purpose
- Core 메모리 사용량 모니터링
- 가용 슬롯 확인
- 메모리 활용도 분석

#### Notes
1. Display Information
   - Current Core Size: 현재 로드된 노드 수
   - Maximum Core Size: 최대 로드 가능 노드 수
   - Available Slots: 남은 슬롯 수
   - Memory Utilization: 메모리 사용률

2. Usage Monitoring
   - 메모리 부족 예방
   - 로드 가능 여부 확인
   - 최적화 필요성 판단

### Delete node
```
delete-node <node_index>
```
Deletes a node and adds it to the garbage chain.

Parameters:
- node_index: Target node to delete (0-255)

Example:
```
> delete-node 5
Successfully deleted node 5

> delete-node 256
Error: node index must be between 0 and 255

> delete-node 0
Error: Cannot delete garbage node (index 0)
```

#### Purpose
- 노드 삭제
- 가비지 체인 관리
- 메모리 정리

#### Notes
1. Garbage Chain
   - 삭제된 노드는 가비지 체인에 추가
   - 가비지 노드(0)는 삭제 불가
   - 재사용을 위한 관리

2. Memory Management
   - Core 배열에서 제거
   - CoreMap 상태 업데이트
   - 메모리 해제

### Validation Commands

#### Validate Free Offsets
```
validate-free-offsets
```
Validates that free block offsets are unique and non-overlapping.

Example:
```
> validate-free-offsets
Free block offsets validated successfully - no overlaps found

> validate-free-offsets extra_args
Error: Invalid arguments
Usage: validate-free-offsets
```

#### Validate cycle
```
validate-cycle <node_index> <channel_index> <axis_number>
```
Checks if there is a cycle (cyclic path) starting from the given node/channel/axis.

Parameters:
- node_index: Starting node (0-255)
- channel_index: Starting channel
- axis_number: Axis to follow

Example:
```
> validate-cycle 0 0 0
Path from node 0, channel 0, axis 0 does not form a cycle

> validate-cycle 1 0 0
Path from node 1, channel 0, axis 0 forms a cycle
```

Error handling:
```
> validate-cycle
Error: Missing arguments
Usage: validate-cycle <node_index> <channel_index> <axis_number>

> validate-cycle 0
Error: Invalid arguments
Usage: validate-cycle <node_index> <channel_index> <axis_number>
```

### cycle Analysis Commands

#### Validate cycle
```shell
validate-cycle <node_index> <channel_index> <axis_number>
```

##### Purpose
Checks if there is a cycle (cyclic path) starting from the given node/channel/axis.

##### Parameters
- node_index: Starting node (0-255)
- channel_index: Starting channel
- axis_number: Axis to follow

##### Example
```shell
> validate-cycle 0 0 0
Path from node 0, channel 0, axis 0 does not form a cycle

> validate-cycle 1 0 0
Path from node 1, channel 0, axis 0 forms a cycle
```

#### Print cycle Information
```shell
print-cycle <node_index> <channel_index> <axis_number>
```

##### Purpose
Prints detailed information about any cycle found starting from the specified node/channel/axis.

##### Parameters
- node_index: Starting node (0-255)
- channel_index: Starting channel
- axis_number: Axis to follow

##### Output Format
1. No cycle Found:
```
No cycle found starting from node 1, channel 0, axis 0
```

2. cycle Found:
```
Found cycle with 3 vertices:
Path: (node 0, Ch 0) -> (node 1, Ch 1) -> (node 2, Ch 0) -> (node 0, Ch 0)
```

##### Error Handling
```shell
> print-cycle
Error: Missing arguments
Usage: print-cycle <node_index> <channel_index> <axis_number>

> print-cycle 0
Error: Invalid arguments
Usage: print-cycle <node_index> <channel_index> <axis_number>
```

### Create cycle Command

#### Command Interface
```shell
create-cycle <node1> <ch1> <node2> <ch2> ... <axis>
```

##### Purpose
Creates a cycle by connecting multiple vertices and channels using the specified axis, after verifying that none of the vertices are already part of a cycle.

##### Parameters
- node1, node2, ...: node indices to connect
- ch1, ch2, ...: Corresponding channel indices
- axis: Axis number to use for connections

##### Process
1. Input Parsing
   - Parse node/channel pairs
   - Extract axis number
   - Validate all parameters

2. cycle Validation
   - Check minimum node count
   - Verify no existing cycles
   - Validate all vertices/channels

3. cycle Creation
   - Create links between consecutive vertices
   - Connect last node back to first
   - Use specified axis for all links

##### Usage Examples
```shell
# Create a 3-node cycle using forward links (axis 0)
> create-cycle 1 0 2 0 3 0 0
Successfully created cycle with 3 vertices

# Attempt to create cycle with node already in cycle
> create-cycle 1 0 4 0 0
Error: node 1 channel 0 already belongs to a cycle on axis 0

# Create a 2-node cycle using backward links (axis 1)
> create-cycle 5 0 6 0 1
Successfully created cycle with 2 vertices

# Error cases
> create-cycle 1 0 1
Error: At least 2 vertices are required to create a cycle

> create-cycle 1 0 256 0 0
Error: Invalid node index
```

##### Notes
1. Limitations
   - Maximum 100 vertices per cycle
   - All links use same axis
   - Vertices must be valid
   - No overlapping cycles on same axis

2. Error Conditions
   - Invalid node indices
   - Invalid channel indices
   - Invalid axis number
   - Too few vertices
   - Missing parameters
   - Existing cycles detected

3. Success Criteria
   - No existing cycles found
   - All links created successfully
   - cycle properly closed
   - All vertices accessible

## Test Commands

### Sequential Token Creation Test
```shell
test-sequential-token
```

#### Purpose
Tests the token creation system by creating strings with incrementally longer strings.

#### Arguments
None. The command runs with predefined test strings.

#### Process
1. Starts with 2-character string
2. Incrementally adds characters
3. Creates string for each string
4. Reports success/failure for each step

#### Output Format
```
Testing sequential token creation...
Creating string for: AB
Successfully created string for AB
Creating string for: ABC
Successfully created string for ABC
...
Sequential token creation tests completed. Failed tests: 0
```

#### Error Cases
- Invalid arguments provided
- string creation failure
- Memory allocation failure

#### Usage Example
```shell
# Run test
> test-sequential-token
Testing sequential token creation...
[Test output...]
Sequential token creation tests completed. Failed tests: 0

# Error case
> test-sequential-token extra_args
Error: Invalid arguments
Usage: test-sequential-token
```

#### Notes
- No arguments should be provided
- Tests complete string creation flow
- Reports total number of failures
- Automatically handles cleanup

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

### Command Handler Organization

#### Test Command Handlers
Test-related commands are handled by dedicated functions in `test_command_handler.c`:

1. Structure
   ```c
   src/cli/
   ├── command_handler.c    // Main command handling
   ├── command_handler.h
   ├── test_command_handler.c  // Test-specific handlers
   └── test_command_handler.h
   ```

2. Benefits
   - Better code organization
   - Separation of concerns
   - Easier maintenance
   - Reduced file size
   - Focused functionality

3. Test Commands
   - test-multiple-link
   - test-create-delete-links
   - test-multi-channel-links
   - test-channel-creation
   - test-axis-create-delete

4. Implementation Pattern
   ```c
   int handle_test_command(char* args) {
       // 1. Parse arguments
       // 2. Validate input
       // 3. Call test function
       // 4. Return result
   }
   ```

#### Command Processing Flow
1. Main command handler receives command
2. If test command, delegates to test handler
3. Test handler parses and validates arguments
4. Test function is called
5. Result is returned to main handler

#### Error Handling
Test commands follow standard error handling:
1. Argument validation
2. Input range checking
3. Consistent error messages
4. Resource cleanup

### node Management Commands

#### Create node
```
create-node
```
Creates a new node with default initialization.

Example:
```
> create-node
Successfully created new node at index 5
```

Error handling:
```
> create-node extra_args
Error: Invalid arguments
Usage: create-node
Example: create-node

> create-node
Error: Maximum number of vertices (256) reached
```

#### Purpose
- 새로운 노드 생성
- 기본 초기화 수행
- 자동 인덱스 할당

#### Notes
1. node Structure
   - 16 bytes 초기 할당
   - 1개의 기본 채널
   - 빈 axis 리스트

2. Memory Management
   - Core 배열에 추가
   - CoreMap 업데이트
   - 파일 저장 자동화

### Test Commands

#### Test Repeating string
```
test-repeating-string
```
Tests string creation with repeating patterns.

Parameters: None

Example:
```shell
> test-repeating-string
Testing repeating string creation...

Testing patterns of length 1
Creating string for: AA (pattern 'A' repeated 2 times)
Successfully created string
...

Testing patterns of length 2
Creating string for: ABAB (pattern 'AB' repeated 2 times)
Successfully created string
...

Repeating string tests completed. Failed tests: 0
```

Error handling:
```shell
> test-repeating-string extra_args
Error: Invalid arguments
Usage: test-repeating-string
```

The command will:
1. Test patterns of length 1-10
2. Test repetition counts 2-10
3. Create strings for all combinations
4. Report success/failure for each test
5. Provide final summary of failed tests

Purpose:
- Verify handling of repeating patterns
- Test system stability with repetitive data
- Ensure proper token reuse and combination
- Validate memory management
- Check cycle creation with repeated elements

[Rest of the document remains the same...] 