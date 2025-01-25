# Vertex Memory Safety Guidelines

## Memory Management Rules

### Vertices Structure
```c
typedef struct {
    Vertex* vertices;
    uint count;
} Vertices;
```

1. Always initialize:
   - Set vertices pointer to NULL
   - Set count to 0 when creating new Vertices

2. Memory Allocation:
   - Check return value from malloc
   - Initialize all members after allocation
   - Set count only after successful allocation

3. Memory Deallocation:
   - Always check for NULL before freeing
   - Set pointer to NULL after freeing
   - Reset count to 0 after freeing

### Common Pitfalls

1. Double Free:
   - Can occur when freeing uninitialized Vertices
   - Happens when freeing already freed memory
   - Results in "double free or corruption (!prev)" error

2. Prevention:
   - Always initialize vertices to NULL
   - Check for NULL before freeing
   - Set to NULL after freeing
   - Validate vertices before operations

### Safe Usage Example
```c
Vertices get_connected_vertices(Vertex vertex, ushort axis_number) {
    Vertices vertices = {NULL, 0};  // Safe initialization
    
    // ... allocation and population ...
    
    if (error_condition) {
        free_vertices(vertices);  // Safe to call even if not allocated
        return (Vertices){NULL, 0};
    }
    
    return vertices;
} 