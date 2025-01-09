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
├── src/            # Source code directory
│   └── tests/      # Test source files
├── obj/            # Object files directory
│   └── src/        # Compiled source objects
├── binary-data/    # Database files directory
│   ├── data.bin    # Node data storage
│   └── map.bin     # Node mapping information
└── system-documents/   # Documentation
```

## Build System

### Object File Management
- Object files are stored in the `obj/` directory
- Directory structure mirrors source structure
- Only modified source files are recompiled
- Build system automatically creates required directories

### Build Commands

#### Basic Build
```bash
make
```
This will:
1. Create necessary object directories
2. Compile only modified source files
3. Link all objects into the `cgdb` executable

#### Clean Build
```bash
make clean
make
```
This removes all compiled files and rebuilds from scratch.

#### Individual Commands
- Compile only: `make cgdb`
- Clean only: `make clean`
- Run program: `make run`

### Compilation Flags
- `-Wall`: Enable all common warnings
- `-Wextra`: Enable extra warnings
- `-g`: Include debug information

## Output Files
- `cgdb`: Main executable
- `obj/*.o`: Object files (organized by directory structure)
- `binary-data/*.bin`: Database files (created at runtime)

## Common Issues
1. Missing directories
   - Build system automatically creates required directories
   - Ensure write permissions in the current directory

2. Compilation errors
   - Check GCC version (recommended: GCC 7.0 or later)
   - Verify all header files are present
   - Check source file organization matches project structure

## Development
When modifying the code:
1. Update header files if adding new functions
2. Only modified source files will be recompiled
3. Use `make clean` if build state becomes inconsistent
4. Test the executable with `./cgdb` or `make run`

## Build Process Details

### Directory Creation
The build system automatically creates:
- `obj/` for all object files
- `obj/src/` for source directory objects
- `obj/src/tests/` for test objects

### Dependency Tracking
- Make tracks source file modifications
- Object files are only rebuilt if source changes
- Header dependencies are automatically tracked

### Performance
- Incremental builds are faster due to object file reuse
- Clean builds create all directories and files fresh
- Directory structure ensures proper organization 