# Build Instructions

## Prerequisites
- GCC compiler
- Make build system
- Standard C library

## Project Structure
```
.
├── CGDB.c          # Main source file
├── CGDB.h          # Header file
├── Makefile        # Build configuration
├── binary-data/    # Database files directory
│   ├── data.bin    # Node data storage
│   └── map.bin     # Node mapping information
└── system-documents/   # Documentation
```

## Build Commands

### Basic Build
```bash
make
```
This will compile the project and create the `cgdb` executable.

### Clean Build
```bash
make clean
make
```
This removes all compiled files and rebuilds from scratch.

### Individual Commands
- Compile only: `make cgdb`
- Clean only: `make clean`

## Compilation Flags
- `-Wall`: Enable all common warnings
- `-Wextra`: Enable extra warnings
- `-g`: Include debug information

## Output Files
- `cgdb`: Main executable
- `*.o`: Object files
- `binary-data/*.bin`: Database files (created at runtime)

## Common Issues
1. Missing binary-data directory
   - The program will create this directory if it doesn't exist
   - Ensure write permissions in the current directory

2. Compilation errors
   - Check GCC version (recommended: GCC 7.0 or later)
   - Verify all header files are present
   - Ensure CGDB.h and CGDB.c are in the same directory

## Development
When modifying the code:
1. Update header file (CGDB.h) if adding new functions
2. Rebuild using `make clean && make`
3. Test the executable with `./cgdb` 