# Channel Management

## Overview
Channel은 노드의 채널 간 연결을 나타내는 메커니즘입니다.

## Channel Creation

### 기본 원칙
1. 순차적 생성
   - Channel 0은 노드 생성 시 자동으로 생성
   - 새로운 채널은 순차적으로 번호 부여
   - 중간 번호 건너뛰기 불가

2. 초기 구조
   - Channel entry (4 bytes): 채널 데이터 오프셋
   - Channel data (2 bytes): axis count 초기화 (0)

### Creation Process
```c
int create_sequential_channel(uint node_index);
```

#### 프로세스
1. 노드 검증
   - 유효한 노드 인덱스 확인
   - Core 배열 존재 여부 확인

2. 공간 계산
   - Channel entry (4 bytes)
   - Channel data (2 bytes)
   - 총 필요 공간: 6 bytes

3. 메모리 관리
   - 필요시 노드 크기 확장
   - 새로운 공간 할당
   - 데이터 복사

4. 데이터 초기화
   - Channel offset 설정
   - Axis count 0으로 초기화
   - Channel count 증가

#### 메모리 레이아웃
```
Before:
[Size Power(2)][Actual Size(4)][Channel Count(2)][Channel 0 Offset(4)][...]

After:
[Size Power(2)][Actual Size(4)][Channel Count(2)][Channel 0 Offset(4)][Channel 1 Offset(4)][...]
                                                 [Channel 1 Data: Axis Count(2)]
```

### Error Handling
- 유효하지 않은 노드 인덱스
- 메모리 할당 실패
- 노드 크기 초과

### Usage Example
```c
// Create new channel in node 0
int result = create_sequential_channel(0);
if (result == CHANNEL_SUCCESS) {
    printf("New channel created\n");
}
```

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

## Command Line Interface

### Channel Creation Command
```
create-channel <node_index>
```

#### Usage
- 노드 인덱스만 지정하면 자동으로 다음 채널 생성
- Channel 0은 노드 생성 시 자동 생성되므로 별도 생성 불필요
- 채널은 순차적으로만 생성 가능

#### Example
```bash
# Create channel 1 in node 0
> create-channel 0
Successfully created new channel in node 0

# Create channel 2 in node 0
> create-channel 0
Successfully created new channel in node 0
```

#### Error Cases
1. 유효하지 않은 노드 인덱스
   ```bash
   > create-channel 256
   Error: Node index must be between 0 and 255
   ```

2. 메모리 할당 실패
   ```bash
   > create-channel 0
   Failed to create channel in node 0
   ``` 

### Channel Creation Process

#### 데이터 구조 변경
1. Channel Entry 추가
   - 새로운 채널의 offset 정보 저장 (4 bytes)
   - 기존 데이터를 4바이트 뒤로 이동
   ```c
   memmove(node + current_offset + 4,          // destination (4 bytes forward)
           node + current_offset,               // source
           current_actual_size - current_offset // size of data to move
   );
   ```

2. Channel Data 추가
   - 노드의 마지막에 axis count 추가 (2 bytes)
   - 초기값 0으로 설정

#### 메모리 레이아웃 변경 예시
```
Initial state (actual_size = N):
[Header(8)][Ch Count(2)][Ch0 Offset(4)][Ch0 Data][Ch1 Data]...

After moving data (actual_size = N + 4):
[Header(8)][Ch Count(2)][Ch0 Offset(4)][New Ch Offset(4)][Ch0 Data][Ch1 Data]...

Final state (actual_size = N + 6):
[Header(8)][Ch Count(2)][Ch0 Offset(4)][New Ch Offset(4)][Ch0 Data][Ch1 Data][New Ch Data(2)]
```

#### 크기 계산 과정
1. 현재 크기 확인
   ```c
   uint current_actual_size = *(uint*)(node + 2);
   uint required_size = current_actual_size + 6;  // channel entry(4) + axis count(2)
   ```

2. 오프셋 계산
   ```c
   uint current_offset = 8 + (*channel_count * 4);  // Header + existing channel offsets
   uint channel_data_offset = current_actual_size;  // New channel data goes at the end
   ```

#### 주의사항
1. 데이터 이동
   - memmove 사용 (메모리 영역 중첩 가능성)
   - 정확한 이동 크기 계산
   - 이동 후 새 데이터 추가

2. 오프셋 관리
   - 새 채널 데이터는 항상 마지막에 추가
   - 채널 엔트리는 기존 엔트리들 사이에 삽입
   - 모든 오프셋 정확성 유지

## Testing

### Sequential Channel Creation Test
```c
int test_sequential_channel_creation(uint node_index);
```

#### Purpose
노드 내에서 채널을 순차적으로 생성하는 기능을 테스트합니다. 채널의 생성, 초기화, 메모리 관리가 올바르게 동작하는지 검증합니다.

#### Test Process
1. 초기 상태 저장
   - 현재 채널 수 기록
   - 노드 상태 확인

2. 순차적 채널 생성
   - 최대 10개 채널까지 테스트
   - 각 채널 생성 후 검증
   - 실패 시 중단

3. 채널별 검증
   - 채널 카운트 확인
   - 채널 오프셋 유효성
   - axis count 초기화 확인

4. 최종 상태 검증
   - 전체 채널 수 확인
   - 메모리 구조 검증

#### 검증 항목
1. Channel Count
   - 순차적 증가
   - 정확한 카운트 유지
   - 최대값 제한 준수

2. Channel Offset
   - 유효한 오프셋 값
   - 순차적 배치
   - 메모리 정렬

3. Initialization
   - axis count 0으로 초기화
   - 메모리 초기화
   - 구조 일관성

4. Memory Management
   - 적절한 공간 할당
   - 노드 크기 조정
   - 메모리 누수 방지

#### 사용 예시
```shell
> test-channel-creation 0
Testing sequential channel creation...
Target Node: 0
Creating channels sequentially...
Final channel count: 10
Channel creation test completed successfully
```

#### 실패 케이스
```shell
> test-channel-creation 0
Testing sequential channel creation...
Target Node: 0
Failed to create channel 5
Channel count mismatch after creation 5: expected 6, got 5
Invalid channel offset for channel 5
Channel creation test completed with 3 failures
```

#### 주의사항
1. 채널 생성
   - 순차적 번호 부여
   - 중간 번호 건너뛰기 불가
   - 최대 개수 제한

2. 메모리 관리
   - 노드 크기 확인
   - 공간 부족 시 확장
   - 포인터 재로드

3. 에러 처리
   - 생성 실패 감지
   - 카운트 불일치
   - 오프셋 오류

## Channel Operations

### Channel Clearing
```c
int clear_channel(uint node_index, ushort channel_index);
```

#### Purpose
채널의 모든 데이터를 제거하고 초기 상태로 되돌립니다.

#### Process
1. 오프셋 계산
   ```c
   uint channel_offset = get_channel_offset(node, channel_index);
   uint channel_end_offset = get_channel_end_offset(node, channel_index);
   ```
   - 현재 채널의 시작과 끝 위치 확인
   - 채널 데이터 크기 계산

2. 데이터 이동
   ```c
   uint move_dest = channel_end_offset + 2;
   uint move_size = actual_size - channel_end_offset;
   memmove(node + move_dest, node + channel_end_offset, move_size);
   ```
   - 다음 채널의 데이터를 앞으로 이동
   - 채널 간 간격 유지
   - 메모리 정렬 보장

3. 크기 업데이트
   ```c
   *(uint*)(node + 2) = actual_size - (channel_end_offset - channel_offset);
   *(ushort*)(node + channel_offset) = 0;
   ```
   - 노드의 실제 크기 감소
   - 채널의 axis count 초기화

4. 오프셋 조정
   ```c
   for (ushort i = channel_index + 1; i < channel_count; i++) {
       *(uint*)(node + 8 + (i * 4)) -= delete_size;
   }
   ```
   - 후속 채널들의 오프셋 업데이트
   - 삭제된 공간만큼 감소
   - 채널 데이터 일관성 유지

#### Return Values
- CHANNEL_SUCCESS: 채널 초기화 성공
- CHANNEL_ERROR: 채널 초기화 실패

#### 주의사항
1. 메모리 관리
   - 데이터 이동 시 오버랩 주의
   - 정확한 크기 계산 필요
   - 메모리 정렬 유지

2. 오프셋 관리
   - 채널 오프셋 정확성 확인
   - 후속 채널 오프셋 업데이트
   - 데이터 일관성 유지

3. 에러 처리
   - 파일 저장 실패 감지
   - 잘못된 채널 인덱스 검증
   - 메모리 접근 오류 방지

#### 사용 예시
```c
// 노드 0의 채널 1 초기화
int result = clear_channel(0, 1);
if (result == CHANNEL_SUCCESS) {
    printf("Channel cleared successfully\n");
} else {
    printf("Failed to clear channel\n");
}
```

### Command Line Interface

#### Clear Channel Command
```shell
clear-channel <node_index> <channel_index>
```

##### 기능
- 지정된 채널의 모든 데이터 제거
- 채널 구조 유지
- 메모리 재구성
- 파일 동기화

##### 사용법
```shell
# 노드 0의 채널 1 초기화
> clear-channel 0 1
Successfully cleared channel 1 in node 0

# 잘못된 노드 인덱스
> clear-channel 256 0
Error: Node index must be between 0 and 255
```

##### 프로세스
1. 입력 검증
   - 노드 인덱스 범위 확인
   - 채널 존재 여부 확인
   - 매개변수 유효성 검사

2. 채널 초기화
   - 모든 데이터 제거
   - axis count 0으로 설정
   - 메모리 재구성

3. 파일 동기화
   - 변경사항 저장
   - 오프셋 업데이트
   - 에러 처리

##### 주의사항
1. 데이터 손실
   - 작업 취소 불가
   - 모든 데이터 영구 삭제
   - 신중한 사용 필요

2. 메모리 관리
   - 정확한 크기 계산
   - 오프셋 재조정
   - 메모리 정렬 유지

[Rest of the document remains the same...] 