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

## Operations

### 1. Axis Creation
```c
int create_axis(int node_index, int channel_index, int axis_number);
```
- 지정된 노드의 채널에 새로운 axis 추가
- Free Space 관리 통합
- 자동 메모리 재할당
- 파일 동기화 처리

### 2. Axis Deletion
```c
int delete_axis(int node_index, int channel_index, int axis_number);
```
- 지정된 axis 삭제
- 남은 axis 데이터 재정렬
- 파일 동기화 처리
- axis count 업데이트

### 3. Axis Information
```c
int get_axis_count(uchar* node, int channel_index);
int get_axis_offset(uchar* node, int channel_index, int axis_number);
bool has_axis(uchar* node, uint channel_offset, int axis_number);
```

## Memory Management
- Axis 삭제 시 메모리 재구성
- 데이터 이동을 통한 공간 최적화
- 파일 동기화 유지

## Error Handling
- 유효하지 않은 노드/채널 인덱스 검사
- 존재하지 않는 axis 삭제 시도 처리
- 메모리 할당 실패 처리
- Free Space 할당 실패 처리

## Usage Example
```c
// Forward link axis 생성
create_axis(node_index, channel_index, AXIS_FORWARD);

// Backward link axis 생성
create_axis(node_index, channel_index, AXIS_BACKWARD);

// Time axis 생성
create_axis(node_index, channel_index, AXIS_TIME);

// Axis 삭제
delete_axis(node_index, channel_index, AXIS_FORWARD);
```

## Implementation Details

### Deletion Process
1. Axis 존재 확인
2. Axis 위치 찾기
3. 남은 데이터 이동
4. Axis count 감소
5. 파일 동기화

### Memory Optimization
- 삭제된 axis의 공간 재활용
- 연속된 메모리 유지
- 효율적인 데이터 접근 