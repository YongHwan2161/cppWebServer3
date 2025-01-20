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
} FreeSpace;
```

## Operations

### Free Block Management
```c
FreeBlock* find_free_block(uint size);
void add_free_block(uint size, long offset);
```

#### Process
1. Block Finding
   - 요청된 크기와 일치하는 블록 검색
   - 정확한 크기 매칭만 허용
   - 효율적인 공간 재사용

2. Block Addition
   - 새로운 블록 정보 추가
   - 블록 배열 크기 조정
   - 파일 동기화 수행

### Memory Management

#### Space Requirements
- Block 정보: 8 bytes per block
  - Size: 4 bytes
  - Offset: 4 bytes
- 메모리 효율성 최적화
- 동적 배열 관리

#### File Format
```
[Block Count(4)][Block Data(8 * count)]
```

### vertex Space Management
```c
uchar* resize_vertex_space(uchar* vertex, uint required_size, int vertex_index, uint* new_size);
```

#### Process
1. 새로운 크기 계산 (2의 제곱수)
2. Free Space에서 적절한 블록 검색
3. 새로운 메모리 할당
4. 데이터 복사
5. 오프셋 관리

#### Parameters
- vertex: 현재 노드 데이터
- required_size: 필요한 최소 크기
- vertex_index: 노드 인덱스
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

### File Persistence
- FREE_SPACE_FILE에 상태 저장
- 프로그램 시작 시 로드
- 변경사항 발생 시 저장

## Usage Example
```c
// vertex resizing
uint new_size;
uchar* new_vertex = resize_vertex_space(old_vertex, required_size, vertex_index, &new_size);
if (new_vertex) {
    // Update Core and recalculate offsets
    Core[vertex_index] = new_vertex;
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

2. Free Block Details
   - Block size in bytes
   - File offset in hexadecimal
   - Sorted by offset

#### Example Output
```
Free Space Information:
Total free blocks: 2

Free Blocks:
Size (bytes)    Offset
------------    ------
16              0x00001000
32              0x00002000
```

### Usage
- Monitor available space
- Debug memory allocation
- Track vertex deletion
- Verify space reclamation 

## Testing

### Resize vertex Space Testing
The resize_vertex_space functionality is tested through a series of operations that verify proper free space management:

#### Test Setup
1. Create multiple axes in vertex 0
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
   - Creates axis in vertex 1
   - Forces vertex resize using 32-byte free block
   - Original 16-byte vertex space returns to free space

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
int test_resize_vertex_space(void);
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

### vertex Size Management

#### Check and Resize vertex
```c
int check_and_resize_vertex(uchar** vertex_ptr, uint required_size, uint vertex_index);
```

##### 기능
- 노드의 크기가 충분한지 검사
- 필요시 자동으로 크기 조정
- 노드 포인터 업데이트

##### Parameters
- vertex_ptr: 노드 포인터의 포인터 (resize 시 업데이트됨)
- required_size: 필요한 최소 크기
- vertex_index: 노드 인덱스

##### Return Values
- FREE_SPACE_SUCCESS: 크기 조정 불필요
- FREE_SPACE_RESIZED: 크기 조정 성공
- FREE_SPACE_ERROR: 크기 조정 실패

##### Process
1. 크기 검사
   - 현재 노드 크기 확인
   - 필요 크기와 비교
   - 충분하면 즉시 반환

2. 크기 조정
   - resize_vertex_space 호출
   - 새로운 메모리 할당
   - 데이터 복사

3. 포인터 업데이트
   - 노드 포인터 갱신
   - Core 배열 업데이트
   - 호출자에게 새 포인터 전달

##### Usage Example
```c
uchar* vertex = Core[vertex_index];
uint required_size = current_size + new_data_size;

int result = check_and_resize_vertex(&vertex, required_size, vertex_index);
if (result == FREE_SPACE_ERROR) {
    // Handle error
    return ERROR_CODE;
}

// vertex pointer may have been updated if resized
// Continue with operation...
```

##### 장점
1. 코드 재사용
   - 공통 크기 조정 로직 중앙화
   - 중복 코드 제거
   - 유지보수성 향상

2. 에러 처리 통합
   - 일관된 에러 처리
   - 명확한 상태 코드
   - 디버깅 용이성

3. 포인터 관리
   - 안전한 포인터 업데이트
   - 일관된 메모리 관리
   - 메모리 누수 방지

##### 주의사항
1. 포인터 전달
   - 이중 포인터 사용
   - resize 후 포인터 업데이트
   - NULL 체크 필수

2. 크기 계산
   - 2의 제곱수 고려
   - 오버플로우 방지
   - 최소 크기 보장

3. 에러 처리
   - 메모리 할당 실패
   - 파일 I/O 오류
   - 상태 일관성 유지

[Rest of the document remains unchanged...] 