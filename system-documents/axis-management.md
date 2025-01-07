# Axis Management

## Overview
Axis는 채널의 연결 속성을 정의하는 메커니즘입니다. 각 채널은 여러 개의 axis를 가질 수 있으며, 각 axis는 고유한 번호로 식별됩니다.

## Data Structure

### Channel Data Layout
```
[Axis Count(2)] [Axis Data(6 * count)]
```

### Axis Data Format (6 bytes)
```
[Axis Number(2)] [Axis Offset(4)]
```

### Axis Content Layout
```
[Link Count(2)] [Link Data(6 * count)]
```

### Memory Example
Channel with 2 axes:
```
Offset    Content          Description
0000      02 00           Axis count (2)
0002      00 00 08 00     Axis 0: number=0, offset=8
0006      01 00 14 00     Axis 1: number=1, offset=20
0008      00 00           Axis 0: link count (0)
...
0014      00 00           Axis 1: link count (0)
```

## Operations

### Axis Creation
```c
int create_axis(int node_index, int channel_index, int axis_number);
```

#### Process
1. 노드/채널 유효성 검사
2. 현재 axis 개수 확인
3. Axis 중복 검사
4. 필요한 공간 계산
   - Axis 데이터 (6 bytes)
   - Link count 초기화 (2 bytes)
5. 공간 부족 시 메모리 재할당
   - resize_node_space 호출
   - 새로운 공간 할당
   - 오프셋 재계산
6. Axis 데이터 작성
   - Axis number 저장
   - Offset 계산 및 저장
   - Link count 초기화
7. Axis count 증가
8. 파일 동기화

### Memory Management

#### Space Requirements
- Axis 데이터: 6 bytes per axis
  - Number: 2 bytes
  - Offset: 4 bytes
- Link count: 2 bytes
- Total: 8 bytes minimum per axis

#### Node Resizing
공간 부족 시 resize_node_space 사용:
1. 새로운 크기 계산
2. Free Space 검색
3. 메모리 재할당
4. 데이터 복사
5. CoreMap 업데이트

## Error Handling
- 유효하지 않은 노드/채널
- Axis 중복
- 메모리 할당 실패
- 파일 I/O 오류

## Usage Example
```c
// Forward link axis 생성
create_axis(node_index, channel_index, AXIS_FORWARD);

// Backward link axis 생성
create_axis(node_index, channel_index, AXIS_BACKWARD);

// Time axis 생성
create_axis(node_index, channel_index, AXIS_TIME);
```

## Implementation Notes
- Axis 생성 시 자동으로 link count 초기화
- 메모리 재할당은 resize_node_space 함수 사용
- 모든 변경사항은 즉시 파일에 동기화
- Free space 정보 자동 업데이트 