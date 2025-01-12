# Map Management

## Overview
Map 관리 시스템은 노드의 파일 내 위치 정보를 관리합니다. 각 노드의 위치는 map.bin 파일에 저장되며, 프로그램 실행 중에는 CoreMap 구조체 배열에서 관리됩니다.

## Data Structure

### CoreMap Entry
```c
typedef struct {
    int core_position;   // Position in Core array (-1 if not loaded)
    int is_loaded;      // 1 if loaded in RAM, 0 if not
    long file_offset;   // Offset position in data.bin
} NodeMapping;
```

### Map File Format
```
[Number of Nodes(4)] [Node 0 Offset(8)] [Node 1 Offset(8)] ... [Node 255 Offset(8)]
```

## Operations

### Save Map Entry
```c
int save_map(uint node_index);
```

#### Implementation Details
1. Binary File Structure
   - Header: 노드 수 (4 bytes)
   - 각 노드별 file_offset (8 bytes each)
   - Total size = 4 + (256 * 8) = 2052 bytes

2. Write Process
   - 정확한 위치 계산: sizeof(uint) + (node_index * sizeof(long))
   - 해당 위치로 직접 이동 (fseek)
   - file_offset 값만 업데이트 (8 bytes)
   - 다른 데이터는 보존

3. Data Preservation
   - 기존 파일 구조 유지
   - 필요한 부분만 수정
   - 다른 노드의 정보는 변경하지 않음

#### Error Handling
- 파일 열기 실패
- Seek 실패
- Write 실패

### Save All Map Entries
```c
int save_map_all(void);
```
전체 CoreMap 정보를 map.bin에 저장합니다.

#### Process
1. map.bin 파일 열기
2. 노드 수 저장 (256)
3. 모든 노드의 offset 저장
4. 파일 닫기

### Load Map
```c
int load_map(void);
```
map.bin에서 모든 노드의 위치 정보를 읽어옵니다.

#### Process
1. map.bin 파일 열기
2. 노드 수 확인
3. 모든 offset 정보 읽기
4. CoreMap 업데이트

### Initialize Map
```c
void init_map(void);
```
CoreMap을 기본값으로 초기화합니다.

## Error Handling
- 파일 열기 실패: MAP_ERROR 반환
- 읽기/쓰기 실패: MAP_ERROR 반환
- 노드 수 불일치: MAP_ERROR 반환

## Core Position Management

### Get Node Position
```c
int get_node_position(uint node_index);
```

#### Purpose
노드의 Core 배열 내 실제 위치를 반환합니다. 노드 인덱스와 Core 배열에서의 위치는 다를 수 있으므로, 이 함수를 통해 실제 위치를 확인해야 합니다.

#### Parameters
- node_index: 위치를 찾을 노드의 인덱스 (0-255)

#### Returns
- 성공: Core 배열에서의 위치 (0 이상)
- 실패: -1 (노드가 로드되지 않았거나 잘못된 인덱스)

#### Process
1. 인덱스 검증
   - 범위 확인 (0-255)
   - 유효하지 않으면 -1 반환

2. 로드 상태 확인
   - CoreMap에서 is_loaded 확인
   - 로드되지 않은 경우 -1 반환

3. 위치 반환
   - CoreMap의 core_position 값 반환
   - 실제 Core 배열 접근에 사용

#### Usage Example
```c
// Get node position and access data
int position = get_node_position(node_index);
if (position >= 0) {
    uchar* node_data = Core[position];
    // Use node_data...
}
```

#### Error Cases
1. 잘못된 노드 인덱스
   ```c
   int pos = get_node_position(256);  // Returns -1
   ```

2. 로드되지 않은 노드
   ```c
   int pos = get_node_position(5);  // Returns -1 if node 5 is not loaded
   ```

### Core Position vs Node Index

#### 차이점
1. Node Index
   - 고정된 식별자 (0-255)
   - 노드의 논리적 주소
   - 파일 저장/로드에 사용

2. Core Position
   - 메모리 내 실제 위치
   - 동적으로 변경 가능
   - 실제 데이터 접근에 사용

#### 사용 패턴
```c
// Wrong way
uchar* node = Core[node_index];  // Don't use index directly

// Correct way
int position = get_node_position(node_index);
if (position >= 0) {
    uchar* node = Core[position];  // Use position
}
```

#### 장점
1. 메모리 효율성
   - 필요한 노드만 로드
   - 연속된 메모리 사용
   - 메모리 단편화 방지

2. 유연성
   - 동적 노드 로드/언로드
   - 메모리 재구성 용이
   - 캐시 효율성 향상

3. 안전성
   - 잘못된 메모리 접근 방지
   - 명시적 에러 처리
   - 상태 추적 용이

## Usage Example
```c
// 단일 노드 맵 정보 저장
if (save_map(node_index) != MAP_SUCCESS) {
    printf("Warning: Failed to update map.bin\n");
}

// 전체 맵 정보 저장
save_map_all();

// 맵 정보 로드
if (load_map() != MAP_SUCCESS) {
    printf("Error: Failed to load map data\n");
}
```

### Command Line Interface

#### Get Node Position Command
```shell
get-node-position <node_index>
```

##### 기능
- Core 배열에서 노드의 실제 위치 조회
- 메모리 주소 정보 표시
- 노드 로드 상태 확인

##### 출력 정보
1. Core Position
   - Core 배열에서의 인덱스
   - 실제 메모리 접근 위치
   - 노드 인덱스와의 차이점 표시

2. Memory Address
   - 실제 메모리 주소
   - 포인터 값 표시
   - 디버깅 정보 제공

##### 사용 예시
```shell
# 정상 케이스
> get-node-position 0
Node 0 is at Core position 0
Memory address: 0x7f8b4c003a00

# 에러 케이스
> get-node-position 256
Error: Invalid node index 256

> get-node-position 5
Error: Node 5 is not loaded in memory
```

##### 주의사항
1. 위치 정보
   - Core position은 동적으로 변경될 수 있음
   - 노드 인덱스와 다를 수 있음
   - 메모리 관리 상태에 따라 변동

2. 메모리 주소
   - 프로그램 실행마다 변경됨
   - 디버깅 목적으로만 사용
   - 직접적인 메모리 접근 지양

### CoreMap Status Monitoring

#### Command Interface
```shell
print-coremap [node_index]
```

##### 기능
- CoreMap의 현재 상태 표시
- 특정 노드의 상세 정보 조회
- 메모리 사용 현황 모니터링

##### 출력 정보
1. 기본 정보 (인덱스 미지정 시)
   - 전체 로드된 노드 수
   - 로드된 노드들의 상태
   - 메모리 점유 현황

2. 노드별 정보
   - 노드 인덱스
   - Core 배열 위치
   - 로드 상태
   - 파일 오프셋

##### 사용 예시
```shell
# 로드된 노드 전체 표시
> print-coremap
CoreMap Status:
Total Loaded Nodes: 3

Node     Core Position   Is Loaded      File Offset
--------------------------------------------------------
0        0              Yes            0x00000000
5        1              Yes            0x00000050
10       2              Yes            0x000000A0

# 특정 노드 정보 표시
> print-coremap 5
CoreMap Status for Node 5:
Node     Core Position   Is Loaded      File Offset
--------------------------------------------------------
5        1              Yes            0x00000050
```

##### 주의사항
1. 출력 형식
   - 정렬된 컬럼 형식
   - 명확한 구분선
   - 가독성 있는 정보 배치

2. 정보 정확성
   - 실시간 상태 반영
   - 일관된 데이터 표시
   - 오프셋 정보 검증

3. 입력 검증
   - 노드 인덱스 범위 확인 (0-255)
   - 숫자 형식 검증
   - 적절한 에러 메시지
``` 