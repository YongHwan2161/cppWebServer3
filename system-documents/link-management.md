# Link Management

## Overview
Link는 노드의 채널 간 연결을 나타내는 메커니즘입니다. 각 링크는 목적지 노드와 채널 정보를 포함하며, 특정 axis를 통해 연결됩니다.

## Data Structure

### Link Data Format (6 bytes)
```
[Node Index(4)] [Channel Index(2)]
```

### Memory Layout
```
[Link Count(2)] [Link 1(6)] [Link 2(6)] ... [Link N(6)]
```

### Memory Example
Axis with 2 links:
```
Offset    Content          Description
0000      02 00           Link count (2)
0002      01 00 00 00     Link 1: Node index (1)
0006      02 00           Link 1: Channel index (2)
0008      02 00 00 00     Link 2: Node index (2)
000C      01 00           Link 2: Channel index (1)
```

## Operations

### Link Creation
```c
int create_link(int source_node, int source_ch, 
               int dest_node, int dest_ch, 
               int axis_number);
```

#### Prerequisites
- 유효한 소스 노드와 채널
- Axis의 존재 (없을 경우 자동 생성)
- 충분한 메모리 공간

#### Process
1. 소스 노드/채널 유효성 검사
2. Axis 존재 여부 확인
   - 없으면 자동 생성
3. 현재 link count 확인
4. 필요한 공간 계산
   - Link 데이터 (6 bytes)
5. 공간 부족 시 메모리 재할당
   - Free Space 검색
   - 새로운 공간 할당
   - 데이터 복사
6. Link 데이터 저장
7. Link count 증가
8. 파일 동기화

### Memory Management

#### Space Requirements
- Link count: 2 bytes
- Link 데이터: 6 bytes per link
  - Node Index: 4 bytes
  - Channel Index: 2 bytes

#### Node Resizing
공간 부족 시:
1. 새로운 크기 계산 (2의 제곱수)
2. Free Space 검색
3. 새로운 공간 할당
4. 기존 데이터 복사
5. 이전 공간 Free Space 반환

## Error Handling
- 유효하지 않은 노드/채널 인덱스
- Axis 생성 실패
- 메모리 할당 실패
- 파일 I/O 오류

## Usage Example
```c
// Forward link 생성
create_link(0, 0,    // source node/channel
           1, 0,     // destination node/channel
           AXIS_FORWARD);  // axis number

// Backward link 생성
create_link(1, 0,    // source node/channel
           0, 0,     // destination node/channel
           AXIS_BACKWARD); // axis number
``` 