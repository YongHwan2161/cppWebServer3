# Vertex Management

## Connected Vertices

### Overview
The system provides functionality to retrieve vertices connected through specific axes, particularly for parent-child relationships.

### Implementation

#### Helper Function
```c
static Vertices get_connected_vertices(Vertex vertex, ushort axis_number)
```

##### Purpose
- Retrieves all vertices connected to a given vertex through a specific axis
- Handles common functionality for both parent and child vertex retrieval
- Manages memory allocation and data extraction

##### Process
1. Validation
   - Check axis existence
   - Verify node loading
   - Validate channel

2. Memory Management
   - Allocate vertex array
   - Handle allocation failures
   - Clean up on errors

3. Data Extraction
   - Calculate offsets
   - Read vertex count
   - Extract vertex data

### Public Functions

#### Get Parent Vertices
```c
Vertices get_parent_vertices(Vertex vertex)
```
- Retrieves all parent vertices using PARENT_AXIS
- Returns empty Vertices struct if no parents exist
- Caller must free vertices.vertices when done

#### Get Child Vertices
```c
Vertices get_child_vertices(Vertex vertex)
```
- Retrieves all child vertices using CHILD_AXIS
- Returns empty Vertices struct if no children exist
- Caller must free vertices.vertices when done

### Usage Example
```c
Vertex current = {node: 5, channel: 0};

// Get parents
Vertices parents = get_parent_vertices(current);
for (int i = 0; i < parents.count; i++) {
    printf("Parent %d: (node %u, channel %u)\n",
           i + 1, parents.vertices[i].node,
           parents.vertices[i].channel);
}
free(parents.vertices);

// Get children
Vertices children = get_child_vertices(current);
for (int i = 0; i < children.count; i++) {
    printf("Child %d: (node %u, channel %u)\n",
           i + 1, children.vertices[i].node,
           children.vertices[i].channel);
}
free(children.vertices);
```

### Memory Management
1. Allocation
   - Dynamic array allocation
   - Size based on vertex count
   - NULL check after allocation

2. Cleanup
   - Caller responsible for freeing
   - Check for NULL before free
   - Clear after use

3. Error Handling
   - Return empty struct on error
   - Clear error messages
   - Memory cleanup on failure

### Benefits
1. Code Reuse
   - Common functionality centralized
   - Reduced code duplication
   - Easier maintenance

2. Consistent Behavior
   - Uniform error handling
   - Standard memory management
   - Common validation process

3. Modularity
   - Single responsibility
   - Clear interface
   - Easy to extend

### node Deletion

#### Command Interface
```shell
delete-node <node_index>
```

##### 기능
- 노드 데이터 초기화
- 가비지 체인에 추가
- 메모리 관리 업데이트

##### 프로세스
1. 유효성 검사
   - 가비지 노드(0) 삭제 방지
   - 가비지 순환 체크
   - 삭제 가능 여부 확인

2. 초기화
   - 노드 데이터를 기본값으로 리셋
   - 기본 채널 구조 유지
   - axis count 초기화

3. 가비지 체인 연결
   - 가비지 노드(0)의 첫 번째 링크 읽기
   - 삭제할 노드를 가비지 노드에 연결
   - 삭제할 노드를 이전 첫 번째 노드에 연결

##### 사용 예시
```shell
# 노드 삭제
> delete-node 5
Successfully deleted node 5

# 가비지 노드 삭제 시도
> delete-node 0
Error: Cannot delete garbage node (index 0)

# 가비지 순환에 포함된 노드 삭제 시도
> delete-node 3
Error: Cannot delete node in garbage cycle
```

##### 주의사항
1. 가비지 체인
   - 가비지 노드(0) 보호
   - 순환 감지 및 방지
   - 체인 일관성 유지

2. 순환 검사
   - 삭제 전 순환 체크
   - 가비지 순환 보호
   - 데이터 일관성 유지 

## Vertex Migration

### Overview
The system provides functionality to migrate vertex relationships when vertices are updated or moved. This ensures referential integrity in the graph structure.

### Implementation

#### Helper Function
```c
static int migrate_vertices_through_axis(Vertex vertex, ushort source_axis, ushort target_axis)
```

##### Purpose
- Updates vertex references in connected vertices
- Maintains bidirectional relationships
- Ensures consistency after vertex changes

##### Process
1. Source Vertex Collection
   - Get vertices connected through source axis
   - Validate connections
   - Handle empty sets

2. Target Reference Update
   - Find matching vertex in target axis
   - Update vertex reference
   - Maintain relationship integrity

3. Memory Management
   - Clean up vertex collections
   - Handle allocation failures
   - Prevent memory leaks

### Public Functions

#### Migrate Parent Vertices
```c
int migrate_parent_vertices(Vertex vertex)
```
- Updates child references in parent vertices
- Maintains parent-child relationships
- Returns SUCCESS on completion

#### Migrate Child Vertices
```c
int migrate_child_vertices(Vertex vertex)
```
- Updates parent references in child vertices
- Maintains child-parent relationships
- Returns SUCCESS on completion

### Usage Example
```c
Vertex updated_vertex = {node: 5, channel: 1};

// Update parent references
if (migrate_parent_vertices(updated_vertex) != SUCCESS) {
    // Handle error
}

// Update child references
if (migrate_child_vertices(updated_vertex) != SUCCESS) {
    // Handle error
}
```

### Memory Management
1. Resource Handling
   - Proper vertex collection cleanup
   - Temporary buffer management
   - Systematic memory release

2. Error Recovery
   - Clean up on partial failures
   - Maintain consistency
   - Prevent resource leaks

### Benefits
1. Code Organization
   - Common functionality centralized
   - Reduced code duplication
   - Simplified maintenance

2. Error Handling
   - Consistent error checking
   - Clear failure points
   - Proper cleanup paths

3. Performance
   - Optimized vertex traversal
   - Minimal memory operations
   - Efficient updates

### Implementation Notes
1. Axis Management
   - Bidirectional updates
   - Relationship preservation
   - Reference integrity

2. Safety Checks
   - Vertex validation
   - Axis existence verification
   - Memory bounds checking

3. Update Process
   - Find matching vertices
   - Update references atomically
   - Maintain consistency 