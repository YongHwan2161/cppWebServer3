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
int create_axis(int vertex_index, int channel_index, int axis_number);
```

#### Process
1. 노드/채널 유효성 검사
2. 현재 axis 개수 확인
3. Axis 중복 검사
4. 필요한 공간 계산
   - Axis 데이터 (6 bytes)
   - Link count 초기화 (2 bytes)
5. 공간 부족 시 메모리 재할당
   - resize_vertex_space 호출
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

#### vertex Resizing
공간 부족 시 resize_vertex_space 사용:
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
create_axis(vertex_index, channel_index, AXIS_FORWARD);

// Backward link axis 생성
create_axis(vertex_index, channel_index, AXIS_BACKWARD);

// Time axis 생성
create_axis(vertex_index, channel_index, AXIS_TIME);
```

## Implementation Notes
- Axis 생성 시 자동으로 link count 초기화
- 메모리 재할당은 resize_vertex_space 함수 사용
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

### Size Calculations
1. Required Size
   ```c
   // Simple size calculation using actual size
   uint current_actual_size = *(uint*)(vertex + 2);
   uint required_size = current_actual_size + 6;  // Add 6 bytes for new axis
   ```

2. Size Validation
   ```c
   // Check if resize needed
   if (required_size > (1 << vertex_size_power)) {
       // Resize to next power of 2
   }
   ```

### Advantages of New Size Management
1. Simplified Calculations
   - Direct access to actual size
   - No need to traverse channels/axes
   - Simple addition for new requirements

2. Memory Efficiency
   - Accurate size tracking
   - Immediate size information
   - No recalculation needed

### Size Update Process
1. Before Adding Axis
   ```
   [Size Power(2)][Actual Size(14)][...]
   ```

2. After Adding Axis
   ```
   [Size Power(2)][Actual Size(20)][...]  // 14 + 6 bytes for new axis
   ```

### Axis Creation Process

#### Memory Layout
1. Axis Entry (6 bytes)
   - Axis Number (2 bytes)
   - Axis Offset (4 bytes)

2. Axis Data
   - Link Count (2 bytes)
   - Link Data (6 bytes per link)

#### Size Calculations
```c
// Calculate required size for new axis
uint current_actual_size = *(uint*)(vertex + 2);
uint required_size = current_actual_size + 8;  // Total new space needed
```

##### Size Breakdown
1. Axis Entry: 6 bytes
   - Axis Number: 2 bytes
   - Axis Offset: 4 bytes

2. Axis Data: 2 bytes
   - Link Count: 2 bytes (초기값 0)

Total: 8 bytes
- 6 bytes (axis entry)
- 2 bytes (link count)

##### 메모리 레이아웃 예시
```
Before:
[vertex Header][Channel Data][Existing Axes]
                                ↑
                         current_actual_size

After:
[vertex Header][Channel Data][Existing Axes][New Axis Entry(6)][Link Count(2)]
                                ↑                             ↑
                         current_actual_size          current_actual_size + 8
```

#### 주의사항
1. 크기 계산
   - Axis Entry (6 bytes)와 Link Count (2 bytes)를 반드시 포함
   - 전체 8 bytes 확보 필요
   - 메모리 정렬 고려

2. 데이터 정렬
   - Axis Entry는 채널 데이터 직후에 위치
   - Link Count는 Axis Data 영역의 시작점에 위치
   - 올바른 오프셋 계산 필수

3. 메모리 관리
   - 충분한 공간 확보
   - 데이터 이동 시 정확한 크기 계산
   - 오프셋 업데이트 정확성

[Rest of the document remains the same...]