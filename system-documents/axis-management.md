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

## Data Layout

### Channel Structure
```
[Axis Count(2)] [Axis Table] [Axis Data]
```

### Axis Table Entry (6 bytes)
```
[Axis Number(2)] [Axis Offset(4)]
```

### Axis Data
```
[Link Count(2)] [Link Data...]
```

## Memory Management

### Axis Creation Process
1. 메모리 레이아웃
   - Channel 시작: Axis Count (2 bytes)
   - Axis Table: (6 bytes * axis 개수)
     - Axis Number (2 bytes)
     - Axis Offset (4 bytes)
   - Axis Data: 각 axis의 실제 데이터

2. 데이터 이동
   - 새 axis 추가 시 기존 axis data를 6바이트 뒤로 이동
   - 모든 axis offset 값을 6바이트씩 증가
   - 새로운 axis entry를 axis table에 추가
   - 새 axis data 영역 초기화

3. 메모리 재할당
   - 필요한 공간 계산
   - 기존 데이터 보존
   - 새로운 공간 할당

### Offset 관리
1. Axis Table
   - 각 entry는 6바이트
   - Offset은 channel 시작점 기준

2. Axis Data
   - 각 axis data는 연속된 공간
   - Axis 추가/삭제 시 offset 조정

### 예시
```
Channel Start
+0000: 02 00           // Axis count (2)
+0002: 00 00 0C 00 00 00   // Axis 0: number=0, offset=12
+0008: 01 00 20 00 00 00   // Axis 1: number=1, offset=32
+000E: [Axis 0 data]
+0022: [Axis 1 data]
```

## Error Handling
- 메모리 할당 실패
- 중복된 axis number
- 파일 I/O 오류 

### Axis Creation Details

#### Memory Layout Calculation
1. 기본 구조
   ```
   [Channel Header(2)] [Axis Table(6*N)] [Axis Data...]
   ```
   - Channel Header: Axis Count (2 bytes)
   - Axis Table Entry: Axis Number(2) + Offset(4)
   - Axis Data: Link Count(2) + Link Data

2. 공간 계산
   ```c
   required_size = channel_offset +          // Channel 시작 위치
                  axis_table_size +          // (현재 axis 수 + 1) * 6
                  last_axis_offset +         // 마지막 axis data 위치
                  last_axis_data_size +      // 마지막 axis의 데이터 크기
                  6 +                        // 새로운 axis table entry
                  2;                         // 새로운 axis의 link count
   ```

#### Data Movement Process
1. Axis Table 확장
   - 새로운 axis entry를 위한 6바이트 공간 확보
   - 기존 axis data를 6바이트 뒤로 이동

2. Offset 업데이트
   - 모든 기존 axis의 offset을 6바이트씩 증가
   - 새로운 axis의 offset 계산
   ```c
   new_axis_offset = last_axis_offset + last_axis_data_size + 6;
   ```

3. 데이터 초기화
   - 새로운 axis entry 작성
     - axis number 저장
     - offset 값 설정
   - axis data 영역 초기화
     - link count = 0

#### Resize 처리
1. 필요 공간 계산
   - 전체 필요 공간 = 기존 데이터 + 새로운 axis 공간
   - 2의 제곱수로 반올림

2. Free Space 관리
   - 적절한 크기의 free block 검색
   - 없으면 새로운 공간 할당
   - 기존 공간은 free space로 반환

3. 데이터 이동
   - 새로운 위치로 데이터 복사
   - CoreMap 업데이트
   - 파일 offset 조정 

### Axis Creation Process Details

#### Channel Offset 계산
1. Channel 위치 찾기
   ```c
   uint channel_offset = get_channel_offset(node, channel_index);
   ```

2. Axis Count 확인
   ```c
   ushort current_count = *(ushort*)(node + channel_offset);
   ```

#### Required Size 계산
1. Axis Table Size
   ```c
   uint axis_table_size = (current_count + 1) * 6;
   ```

2. 필요 공간 계산
   - 첫 번째 axis 생성 시:
   ```c
   required_size = channel_offset + axis_table_size + 4;
   // +2 for axis count
   // +2 for initial link count
   ```
   
   - 기존 axis가 있는 경우:
   ```c
   required_size = channel_offset + last_axis_offset + 
                  last_axis_data_size + 6 + 2;
   ```

#### Offset 계산
1. Channel 기준 상대 offset
   - 첫 번째 axis:
   ```c
   new_axis_offset = 2 + 6;  // After axis count and its table entry
   ```
   
   - 추가 axis:
   ```c
   new_axis_offset = 2 + axis_table_size;  // After all table entries
   ```

2. 데이터 이동
   - 기존 데이터를 6바이트 뒤로 이동
   - 모든 axis offset 값 6바이트 증가

#### 메모리 구조
```
[Channel Start]
+0: Axis Count (2)
+2: Axis Table (6 * N)
   - Axis Number (2)
   - Relative Offset (4)
+2+6N: Axis Data
   - Link Count (2)
   - Link Data (...)
``` 

### Data Movement Strategy

#### Axis Data Movement
1. 단순화된 이동 방식
   ```c
   uint data_start = channel_offset + 2 + (current_count * 6);  // Axis data 시작점
   uint data_size = current_node_size - data_start - 6;         // 전체 남은 데이터
   ```

2. 이동 과정
   - Axis table 뒤의 모든 데이터를 6바이트 뒤로 이동
   - 메모리 이동은 한 번의 memmove로 처리
   - 복잡한 크기 계산 불필요

3. Offset 업데이트
   - 모든 axis의 offset을 6바이트씩 증가
   - Table entry 순서대로 순차 처리

#### 장점
1. 단순성
   - 복잡한 크기 계산 제거
   - 전체 데이터를 한 번에 이동
   - 실수 가능성 감소

2. 효율성
   - 메모리 접근 최소화
   - 불필요한 계산 제거
   - 코드 가독성 향상

#### 메모리 레이아웃
```
[Channel Start]
+0: Axis Count (2)
+2: Axis Table (6 * N)
+2+6N: All Axis Data (moved as one block)
``` 

### Axis Offset 계산

#### 첫 번째 Axis (axis_count = 0)
```
new_axis_offset = 8
```
- axis count (2 bytes)
- first axis table entry (6 bytes)
- 첫 번째 axis는 항상 고정된 위치에 생성

#### 추가 Axis (axis_count > 0)
```
new_axis_offset = required_size - channel_offset - 2
```
구성:
1. required_size: 전체 필요한 공간
2. channel_offset: 채널 시작 위치
3. -2: 새로운 axis의 link count 공간

#### 계산 장점
1. 단순성
   - 이미 계산된 required_size 활용
   - 복잡한 link count 계산 불필요
   - 실수 가능성 감소

2. 효율성
   - 중복 계산 제거
   - 코드 가독성 향상
   - 유지보수 용이

### Axis 삭제 프로세스

#### 기본 검증
1. 노드 유효성 검사
2. Axis 존재 여부 확인
3. Axis 위치 찾기

#### 삭제할 데이터 계산
```c
uint bytes_to_remove = 6 + 2 + (link_count * 6);
```
구성:
- Axis entry (6 bytes)
- Link count (2 bytes)
- Link data (6 bytes * link_count)

#### 데이터 이동 처리
1. 마지막 Axis인 경우
   - Axis entry만 제거
   - Axis count 감소

2. 중간 Axis 삭제
   - 뒤쪽 Axis 데이터를 앞으로 이동
   - 남은 Axis들의 offset 업데이트
   - Axis entry 제거
   - Axis count 감소

#### 최적화 포인트
1. 메모리 관리
   - Resize 불필요 (데이터 감소)
   - 한 번의 memmove로 데이터 이동
   - 불필요한 메모리 할당 없음

2. 데이터 이동
   - 정확한 이동 크기 계산
   - 효율적인 데이터 이동
   - Offset 정확한 업데이트

3. 파일 동기화
   - 변경된 데이터만 저장
   - 단일 파일 쓰기 작업

#### 에러 처리
1. 입력 검증
   - 유효하지 않은 노드
   - 존재하지 않는 Axis
   - 파일 I/O 오류

2. 반환 값
   - AXIS_SUCCESS: 삭제 성공
   - AXIS_ERROR: 삭제 실패

### Memory Layout Details

#### Offset Calculations
1. Channel Offset
   - Base offset from node start
   - Used as reference for all channel data
   - Calculated using get_channel_offset()

2. Axis Offset
   - Relative to channel offset
   - Points to axis data within channel
   - Must be added to channel_offset for absolute position

3. Link Data Access
   - Link count location: channel_offset + axis_offset
   - Link data starts: channel_offset + axis_offset + 2
   - Each link entry: 6 bytes (4 for node, 2 for channel)

Example memory layout:
```
Node start
+0000: Node header
+0002: Channel count
+0004: Channel offsets
      |
      v
Channel start (channel_offset)
+0008: Axis count
+000A: Axis entries
      |
      v
Axis data (channel_offset + axis_offset)
+0010: Link count
+0012: Link entries
```

#### Access Pattern
```c
// Get link count for axis
ushort link_count = *(ushort*)(node + channel_offset + axis_offset);

// Access link data
for (int i = 0; i < link_count; i++) {
    int link_offset = channel_offset + axis_offset + 2 + (i * 6);
    uint dest_node = *(uint*)(node + link_offset);
    ushort dest_channel = *(ushort*)(node + link_offset + 4);
}
```

### Axis Offset Functions

#### get_axis_offset
```c
uint get_axis_offset(uchar* node, ushort channel_index, ushort axis_number);
```
특정 axis의 offset을 반환합니다.
- 입력: 노드, 채널 인덱스, axis 번호
- 반환: axis의 offset 또는 -1 (axis가 없는 경우)

#### get_last_axis_offset
```c
int get_last_axis_offset(uchar* node, ushort channel_index);
```
채널의 마지막 axis offset을 반환합니다.
- 입력: 노드, 채널 인덱스
- 반환: 마지막 axis의 offset 또는 -1 (axis가 없는 경우)

##### 구현 세부사항
1. 채널 오프셋 획득
   ```c
   uint channel_offset = get_channel_offset(node, channel_index);
   ```

2. Axis 개수 확인
   ```c
   ushort axis_count = *(ushort*)(node + channel_offset);
   if (axis_count == 0) return -1;
   ```

3. 마지막 Axis의 오프셋 획득
   ```c
   int axis_data_offset = channel_offset + 2;  // Skip axis count
   uint last_axis_offset = *(uint*)(node + axis_data_offset + ((axis_count - 1) * 6) + 2);
   ```

##### 사용 예시
```c
// 마지막 axis의 offset 획득
int last_offset = get_last_axis_offset(node, channel_index);
if (last_offset < 0) {
    // 에러 처리: axis가 없음
} else {
    // 마지막 axis 데이터 접근
    ushort* link_count = (ushort*)(node + channel_offset + last_offset);
}
```

##### 주의사항
1. 오프셋 계산
   - axis table entry: 6 bytes (number: 2, offset: 4)
   - 마지막 axis index: axis_count - 1
   - offset 위치: entry 시작 + 2 bytes

2. 에러 처리
   - axis가 없는 경우 -1 반환
   - 반환값 검사 필수