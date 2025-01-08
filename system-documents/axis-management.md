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