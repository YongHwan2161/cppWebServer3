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
5. 이전 공간 Free Space에 반환
6. CoreMap 업데이트

#### Parameters
- node: 현재 노드 데이터
- required_size: 필요한 최소 크기
- node_index: 노드 인덱스
- new_size: 할당된 새로운 크기 (출력)

#### Returns
- 성공: 새로운 노드 데이터 포인터
- 실패: NULL

### Free Block Management
```c
FreeBlock* find_free_block(uint size);
void add_free_block(uint size, long offset);
```

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