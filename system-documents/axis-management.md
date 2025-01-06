# Axis Management

## Overview
Axis는 채널 간의 연결(link) 속성을 정의하는 메커니즘입니다. 각 채널은 여러 개의 axis를 가질 수 있으며, 각 axis는 고유한 번호로 식별됩니다.

## Axis Structure
### Channel Data Format
```
[Axis Count(2)] [Axis 1(6)] [Axis 2(6)] ... [Axis N(6)]
```

### Axis Data Format
```
[Axis Number(2)] [Data Offset(4)]
```

## Memory Management

### 1. Space Allocation
- Axis 추가 시 필요한 공간 계산
- 노드 크기가 부족한 경우:
  1. Free Space에서 적절한 크기의 블록 검색
  2. 적합한 블록이 있으면 해당 공간 사용
  3. 없으면 새로운 공간 할당 후 기존 공간을 Free Space에 반환

### 2. Node Resizing Process
1. 새로운 크기 계산 (2의 제곱수)
2. Free Space 검색
3. 데이터 복사 및 이동
4. CoreMap 업데이트
5. 이전 공간 Free Space 등록

### 3. File Synchronization
- 노드 데이터 변경 시 data.bin 업데이트
- Free Space 변경 시 free_space.bin 업데이트
- CoreMap의 offset 정보 유지

## Common Axis Types
```c
#define AXIS_FORWARD 0    // Forward links
#define AXIS_BACKWARD 1   // Backward links
#define AXIS_TIME 3      // Time-related data
```

## Key Functions

### 1. Axis Creation
```c
int create_axis(int node_index, int channel_index, int axis_number);
```
- 지정된 노드의 채널에 새로운 axis 추가
- Free Space 관리 통합
- 자동 메모리 재할당
- 파일 동기화 처리

### 2. Axis Information
```c
int get_axis_count(uchar* node, int channel_index);
int get_axis_offset(uchar* node, int channel_index, int axis_number);
bool has_axis(uchar* node, uint channel_offset, int axis_number);
```

## Error Handling
- 유효하지 않은 노드/채널 인덱스 검사
- 메모리 할당 실패 처리
- axis 중복 검사
- Free Space 할당 실패 처리

## Usage Example
```c
// Forward link axis 생성
create_axis(node_index, channel_index, AXIS_FORWARD);

// Backward link axis 생성
create_axis(node_index, channel_index, AXIS_BACKWARD);

// Time axis 생성
create_axis(node_index, channel_index, AXIS_TIME);
``` 