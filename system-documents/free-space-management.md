# Free Space Management

## Overview
Free Space Manager는 데이터 파일 내의 사용 가능한 공간을 관리하는 시스템입니다.

## Components

### Data Structures
```c
typedef struct {
    uint size;    // Block size in bytes
    long offset;  // File offset
} FreeBlock;

typedef struct {
    uint count;          // Number of free blocks
    FreeBlock* blocks;   // Array of free blocks
    uint* free_indices;  // Array of free node indices
    uint index_count;    // Number of free indices
} FreeSpace;
```

## Operations

### Node Space Management
```c
uchar* resize_node_space(uchar* node, uint required_size, int node_index, uint* new_size);
```

#### Process
1. 새로운 크기 계산 (2의 제곱수)
2. Free Space에서 적절한 블록 검색
3. 새로운 메모리 할당
4. 데이터 복사
5. 오프셋 관리
   ```c
   // 올바른 순서
   long old_offset = CoreMap[node_index].file_offset;  // 기존 오프셋 저장
   CoreMap[node_index].file_offset = free_block->offset;  // 새 오프셋 설정
   add_free_block(current_size, old_offset);  // 기존 공간 반환
   ```

6. 메모리 정리
   - 할당된 FreeBlock 해제
   - 이전 노드 메모리 해제

#### Parameters
- node: 현재 노드 데이터
- required_size: 필요한 최소 크기
- node_index: 노드 인덱스
- new_size: 할당된 새로운 크기 (출력)

#### Returns
- 성공: 새로운 노드 데이터 포인터
- 실패: NULL

#### 주의사항
1. 오프셋 관리
   - CoreMap 업데이트 전 기존 오프셋 저장
   - 올바른 오프셋으로 free space 추가
   - 데이터 일관성 유지

2. 메모리 관리
   - FreeBlock 메모리 누수 방지
   - 적절한 시점의 메모리 해제

### Free Block Management
```c
FreeBlock* find_free_block(uint size);
void add_free_block(uint size, long offset);
```

#### Find and Get Free Block
```c
FreeBlock* find_and_get_free_block(uint size);
```

##### Process
1. Block 검색
   - 요청된 크기와 일치하는 block 찾기
   - blocks 배열 순차 검색

2. Block 반환 및 제거
   - 찾은 block 정보 복사
   - free space list에서 제거
   - 메모리 재할당

##### 반환 값
- 성공: 할당된 FreeBlock 포인터
- 실패: NULL

##### 메모리 관리
1. 반환 Block
   - 새로운 메모리 할당
   - block 정보젵 복사
   - 호출자가 메모리 해제 책임

2. Free Space 관리
   - blocks 배열 크기 조정
   - 빈 공간 제거
   - 메모리 최적화

##### 사용 예시
```c
FreeBlock* block = find_and_get_free_block(required_size);
if (block) {
    // Use the block
    // ...
    free(block);  // Don't forget to free
}
```

##### 주의사항
1. 메모리 관리
   - 반환된 block은 사용 후 반드시 해제
   - free space 메모리 관리
   - 메모리 누수 방지

2. 동기화
   - free space 정보 갱신
   - 파일 저장 고려

### Free Index Management
```c
int get_free_index();
void add_free_index(uint index);
```

## File Persistence
- FREE_SPACE_FILE에 상태 저장
- 프로그램 시작 시 로드
- 변경사항 발생 시 저장

## Usage Example
```c
// Node resizing
uint new_size;
uchar* new_node = resize_node_space(old_node, required_size, node_index, &new_size);
if (new_node) {
    // Update Core and recalculate offsets
    Core[node_index] = new_node;
    // ...
}
```

## Error Handling
- 메모리 할당 실패
- 파일 I/O 오류
- 적절한 Free Block 없음 

## Monitoring

### Free Space Information
```c
int handle_print_free_space(void);
```

#### Display Format
1. Summary Information
   - Total number of free blocks
   - Number of free node indices

2. Free Block Details
   - Block size in bytes
   - File offset in hexadecimal
   - Sorted by offset

3. Free Node Indices
   - List of available node indices
   - Used for new node allocation

#### Example Output
```
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

### Usage
- Monitor available space
- Debug memory allocation
- Track node deletion
- Verify space reclamation 

## Testing

### Resize Node Space Testing
The resize_node_space functionality is tested through a series of operations that verify proper free space management:

#### Test Setup
1. Create multiple axes in node 0
   ```shell
   create-axis 0 0 1
   create-axis 0 0 2
   create-axis 0 0 3
   ```
   This generates initial free blocks:
   - 16 bytes at offset 0x00000000
   - 32 bytes at offset 0x00001000

#### Test Scenario
1. Initial State Verification
   - Confirms presence of two free blocks
   - Verifies correct block sizes

2. Resize Trigger
   - Creates axis in node 1
   - Forces node resize using 32-byte free block
   - Original 16-byte node space returns to free space

3. Final State Verification
   - Checks for two 16-byte free blocks
   - Verifies proper reuse of free blocks
   - Confirms correct free space management

#### Expected Results
Initial Free Space:
```
Free Blocks:
Size (bytes)    Offset
------------    ------
16              0x00000000
32              0x00001000
```

Final Free Space:
```
Free Blocks:
Size (bytes)    Offset
------------    ------
16              0x00000000
16              0x00000010
```

#### Test Implementation
```c
int test_resize_node_space(void);
```
- Verifies free block allocation
- Checks free space reclamation
- Validates block size tracking
- Confirms offset management

### Free Block Offset Testing
The system provides a test to verify that free blocks never have overlapping offsets:

#### Test Command
```shell
test-free-offsets
```

#### Test Process
1. Checks each pair of free blocks for:
   - Identical offsets
   - Overlapping ranges (offset + size)
2. Reports any conflicts found:
   - Block indices
   - Conflicting offsets
   - Block sizes involved

#### Example Output
Success case:
```
Testing free block offset uniqueness...
Checking 3 free blocks for offset conflicts...
✓ All free block offsets are unique and non-overlapping
Free block offset tests completed: 0 failed
```

Error case:
```
Testing free block offset uniqueness...
Checking 3 free blocks for offset conflicts...
✗ Conflict found: Blocks 0 and 2 have same offset 0x00001000
✗ Overlap found between blocks 1 and 2
  Block 1: offset=0x00002000, size=32
  Block 2: offset=0x00002010, size=16
Free block offset tests completed: 2 failed
```

#### Implementation Details
The test verifies:
1. No two blocks share the same offset
2. No block's range (offset to offset+size) overlaps with another
3. All blocks maintain proper boundaries

This ensures the integrity of the free space management system by preventing:
- Double allocation of same space
- Corruption of free block boundaries
- Invalid memory access

[Rest of the document remains unchanged...] 