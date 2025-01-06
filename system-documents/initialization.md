# System Initialization

## Overview
The initialization process ensures all necessary components are properly set up when the program starts. This includes checking and creating required directories and files, loading existing data, or creating new data structures as needed.

## Initialization Process

### 1. Directory Structure
- Checks for existence of `binary-data` directory
- Creates directory if it doesn't exist
- Ensures proper permissions are set

### 2. Database Files
- **map.bin**: Node mapping information
  - Created if doesn't exist
  - Loaded into CoreMap if exists
- **data.bin**: Node data storage
  - Created with initial nodes if doesn't exist
  - Used to load existing nodes if present
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
   - Loads initial set of nodes
   - Prepares for dynamic node loading

3. Free Space Management
   - Initializes free space tracking
   - Loads existing free space data
   - Creates new free space file if needed

## Implementation Details

### Key Functions
```c
int initialize_system(void);        // Main initialization function
int check_and_create_directory(void);  // Directory setup
int initialize_database(void);      // Database initialization
void cleanup_system(void);         // System cleanup
```

### Return Codes
```c
#define INIT_SUCCESS 0    // Successful initialization
#define INIT_NEW_DB 1     // New database created
#define INIT_ERROR -1     // Initialization failed
```

### Cleanup Process
- Frees all allocated memory
- Saves current state to files
- Ensures proper system shutdown

## Error Handling
- Directory creation failures
- File access issues
- Memory allocation errors
- Data corruption detection

## Usage Example
```c
int main() {
    int init_status = initialize_system();
    
    if (init_status == INIT_ERROR) {
        printf("System initialization failed\n");
        return 1;
    }
    
    // Program logic...
    
    cleanup_system();
    return 0;
}
``` 