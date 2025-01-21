# System Initialization

## Overview
The initialization process ensures all necessary components are properly set up when the program starts. This includes checking and creating required directories and files, loading existing data, or creating new data structures as needed.

## Initialization Process

### 1. Directory Structure
- Checks for existence of `binary-data` directory
- Creates directory if it doesn't exist
- Ensures proper permissions are set

### 2. Database Files
- **map.bin**: node mapping information
  - Created if doesn't exist
  - Loaded into CoreMap if exists
- **data.bin**: node data storage
  - Created with initial vertices if doesn't exist
  - Used to load existing vertices if present
- **free_space.bin**: Free space management
  - Created with empty state if doesn't exist
  - Loaded into memory if exists

### 3. Memory Structures
1. CoreMap Initialization
   - Allocates memory for mapping structure
   - Loads existing mapping data if available
   - Initializes with default values if new

2. Core Array Setup
   - Allocates memory for node pointers
   - Loads initial set of vertices
   - Prepares for dynamic node loading

3. Free Space Management
   - Initializes free space tracking
   - Loads existing free space data
   - Creates new free space file if needed

## Testing Structure

### Test Files Organization
```
src/
├── tests/
│   ├── axis_tests.h     # Test function declarations
│   └── axis_tests.c     # Test implementations
└── ...
```

### Test Functions
Each test module contains:
1. Individual test cases
2. Success/failure reporting
3. Expected behavior verification
4. Error case testing

### Axis Tests
Located in `src/tests/axis_tests.c`:
```c
int test_axis_creation(void);  // Returns number of failed tests
```

#### Test Cases
1. Basic Axis Creation
   - Forward axis
   - Backward axis
   - Time axis

2. Error Handling
   - Duplicate axis creation
   - Invalid node index

3. Success Criteria
   - Successful creation returns AXIS_SUCCESS
   - Proper error handling returns AXIS_ERROR
   - Appropriate warning messages displayed

### Running Tests
```c
int main() {
    initialize_system();
    
    // Run axis tests
    int failed_tests = test_axis_creation();
    
    cleanup_system();
    return failed_tests;
}
```

## Testing

### Test Commands
The following commands are available for testing:

1. Run All Tests
```shell
run-tests
```
Executes all available test cases including:
- Axis creation tests
- Resize node space tests
- Reports total number of failed tests

2. Test Resize Functionality
```shell
test-resize
```
Specifically tests the resize_node_space functionality:
- Creates initial test conditions
- Verifies free space management
- Checks block allocation and reuse
- Reports test results

3. Test Axis Creation/Deletion
```shell
test-axis-create-delete <node> <channel> <max_axis>
```
Tests axis creation and deletion cycle:
- Creates axes from 0 to max_axis
- Verifies all axes were created
- Deletes all axes
- Verifies complete deletion
- Reports any failures

Parameters:
- node: Target node index (0-255)
- channel: Target channel index
- max_axis: Maximum axis number to create

Example:
```shell
> test-axis-create-delete 0 0 5
Testing axis creation and deletion (max axis: 5)...
Creating axes 0 to 5...
Verifying created axes...
Deleting all axes...
Verifying axis deletion...
Axis creation/deletion tests completed: 0 failed
```

### Test Output Format
Tests provide detailed feedback:
```