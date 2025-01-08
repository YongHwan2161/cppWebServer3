# Channel Management

## Overview
Channel은 노드 내의 독립적인 데이터 영역을 나타냅니다. 각 노드는 하나 이상의 채널을 가질 수 있으며, 각 채널은 자체적인 axis 집합을 가질 수 있습니다.

## Channel Structure
### Node Data Format
```
[Node Size(2)] [Channel Count(2)] [Channel Offsets(4 * N)] [Channel Data...]
```

### Channel Data Format
```
[Axis Count(2)] [Axis Data...]
```

## Key Functions

### 1. Channel Information
```c
int get_channel_count(uchar* node);
uint get_channel_offset(uchar* node, int channel_index);
int get_channel_size(uchar* node, int channel_index);
```
- 채널 개수 조회
- 채널 위치 계산
- 채널 크기 계산

### 2. Channel Creation
```c
int create_channel(int node_index);
```
- 새로운 채널 생성
- 필요시 노드 크기 자동 조정
- 채널 초기화 (0 axes)

## Memory Management
1. 채널 오프셋
   - 각 채널의 위치는 4바이트 오프셋으로 저장
   - 오프셋은 노드 시작점 기준

2. 크기 계산
   - 기본 채널 크기: 2바이트 (axis count)
   - 채널 오프셋 테이블: 4바이트 * 채널 수
   - 전체 노드 크기는 2의 제곱수로 유지

3. 메모리 재할당
   - 새 채널 추가 시 필요한 경우 노드 크기 확장
   - 기존 데이터 보존
   - 새 채널 초기화

## Error Handling
- 유효하지 않은 노드 인덱스 검사
- 유효하지 않은 채널 인덱스 검사
- 메모리 할당 실패 처리

## Usage Example
```c
// Get channel information
int channel_count = get_channel_count(node);
uint channel_offset = get_channel_offset(node, 0);
int channel_size = get_channel_size(node, 0);

// Create new channel
int result = create_channel(node_index);
if (result == CHANNEL_SUCCESS) {
    printf("New channel created\n");
}
``` 