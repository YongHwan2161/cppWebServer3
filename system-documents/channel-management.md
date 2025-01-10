# Channel Management

## Overview
Channel은 노드의 채널 간 연결을 나타내는 메커니즘입니다.

## Memory Layout

### Node Header Structure
```
[Size Power(2)][Actual Size(4)][Channel Count(2)][Channel Offsets(4*N)]
```

### Memory Layout Details
1. Header Section (0-11)
   - 0-1: Size Power (2 bytes)
   - 2-5: Actual Size (4 bytes)
   - 6-7: Channel Count (2 bytes)
   - 8-11: First Channel Offset (4 bytes)

2. Channel Data Section (12+)
   - 12-13: First Channel's Axis Count
   - 14+: Channel Data

### Offset Calculations
1. Channel Count 위치
   ```c
   ushort channel_count = *(ushort*)(node + 6);  // Skip size power(2) + actual size(4)
   ```

2. Channel Offset Table
   ```c
   uint channel_offset = *(uint*)(node + 8 + (channel_index * 4));
   // 8 = size power(2) + actual size(4) + channel count(2)
   ```

### Memory Example
```
Offset  Content     Description
0-1     04 00      Size Power (2^4 = 16 bytes)
2-5     0E 00 00 00 Actual Size (14 bytes)
6-7     01 00      Channel Count (1)
8-11    0C 00 00 00 Channel 0 Offset (12)
12-13   00 00      Channel 0: Axis Count (0)
14-15   00 00      Reserved
```

### Channel Offset Calculation
```c
// First channel always starts at byte 12
uint first_channel_offset = 12;  // 2(size) + 4(actual) + 2(count) + 4(offset)

// Subsequent channels
uint next_channel_offset = previous_channel_end;
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