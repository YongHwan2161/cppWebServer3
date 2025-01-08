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