# Axis Management

## Overview
Axis는 채널의 연결 속성을 정의하는 메커니즘입니다. 각 채널은 여러 개의 axis를 가질 수 있으며, 각 axis는 고유한 번호로 식별됩니다.

## Axis Types

### 1. Token Search Axis (0)
- 토큰 데이터 검색을 위한 이진 트리 구조
- 각 axis는 정확히 2개의 link entry 보유
- 트리 구조를 통한 효율적인 토큰 데이터 검색
- 데이터 탐색 시 사용되는 주요 axis

### 2. Token Data Axis (1)
- 데이터 배열의 토큰 분리에 사용
- 기존 토큰 검색 시 활용
- 토큰 데이터의 효율적인 관리
- 데이터 분할 및 재사용 최적화

### 3. String Axis (2)
- 토큰들을 연결하여 긴 문자열 저장
- 순환 구조(cycle)를 통한 데이터 연결
- 토큰 데이터의 순차적 접근
- 문자열 데이터의 효율적인 저장 및 관리

### 4. Property Axis (3)
- 버텍스의 특성 정의
- 노드 인덱스를 통한 속성 표현
- 채널 0을 기본값으로 사용
- 확장 가능한 속성 시스템

#### Property Examples
- Node 0 참조: 문자열 시작점 표시
- 다양한 노드 인덱스로 속성 확장 가능
- 데이터 타입별 특성 정의
- 시스템 확장성 보장

### 5. String Parent Axis (4)
- 문자열의 부모-자식 관계에서 부모를 가리키는 axis
- 각 문자열은 하나의 부모만 가질 수 있음
- Root 문자열은 이 axis를 사용하지 않음
- 계층 구조의 상향 탐색에 사용

### 6. String Child Axis (5)
- 문자열의 자식들을 가리키는 axis
- 하나의 문자열은 여러 자식을 가질 수 있음
- N-ary tree 구조 구현
- 계층 구조의 하향 탐색에 사용

### Hierarchical String Structure
1. Root String
   - 데이터베이스 생성 시 자동 생성
   - STRING_PARENT_AXIS 없음
   - 모든 문자열의 최상위 조상

2. Parent-Child Relations
   - 각 문자열은 하나의 부모를 가짐
   - 여러 자식 문자열 보유 가능
   - 디렉토리 구조와 유사한 계층

3. Navigation
   - STRING_PARENT_AXIS로 부모 탐색
   - STRING_CHILD_AXIS로 자식들 탐색
   - 효율적인 계층 구조 순회

### Implementation Example
```c
// Create child string
create_axis(child_node, channel, STRING_PARENT_AXIS);
create_link(child_node, channel, parent_node, parent_channel, STRING_PARENT_AXIS);

// Add child to parent
create_axis(parent_node, channel, STRING_CHILD_AXIS);
create_link(parent_node, channel, child_node, child_channel, STRING_CHILD_AXIS);
```

### Usage Notes
1. Parent Axis (4)
   - 단일 링크만 허용
   - Root에서는 사용하지 않음
   - 부모 탐색용

2. Child Axis (5)
   - 다중 링크 허용
   - 자식 순서 유지
   - 하위 구조 탐색용

3. 구조 관리
   - 부모-자식 관계 일관성 유지
   - 순환 참조 방지
   - 계층 깊이 제한 없음

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
#define TOKEN_SEARCH_AXIS 0
#define TOKEN_DATA_AXIS 1
#define STRING_AXIS 2
#define PROPERTY_AXIS 3

int create_axis(uint node_index, ushort channel, ushort axis_number);
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

#### node Resizing
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
// Create token search axis
create_axis(node_index, channel, TOKEN_SEARCH_AXIS);

// Create string axis for cycle
create_axis(node_index, channel, STRING_AXIS);

// Set vertex property
create_axis(node_index, channel, PROPERTY_AXIS);
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

### Size Calculations
1. Required Size
   ```c
   // Simple size calculation using actual size
   uint current_actual_size = *(uint*)(node + 2);
   uint required_size = current_actual_size + 6;  // Add 6 bytes for new axis
   ```

2. Size Validation
   ```c
   // Check if resize needed
   if (required_size > (1 << node_size_power)) {
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
uint current_actual_size = *(uint*)(node + 2);
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
[node Header][Channel Data][Existing Axes]
                                ↑
                         current_actual_size

After:
[node Header][Channel Data][Existing Axes][New Axis Entry(6)][Link Count(2)]
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