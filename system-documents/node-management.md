# node Management

## File Operations

### node Data Persistence
노드 데이터의 파일 저장을 담당하는 중앙화된 함수를 제공합니다.

#### save_node_to_file
```c
bool save_node_to_file(unsigned int node_index);
```

##### 기능
- 노드 데이터를 data.bin 파일에 저장
- map.bin의 offset 정보 업데이트
- 필요시 파일 및 디렉토리 자동 생성

##### 프로세스
1. 디렉토리 및 파일 생성
   - DATA_DIR 디렉토리 생성 (없는 경우)
   - data.bin 파일 생성 (없는 경우)
   - map.bin 파일 생성 (없는 경우)

2. data.bin 작업
   - 파일 열기 (r+b 모드)
   - 없으면 새로 생성 (wb 모드)
   - CoreMap offset으로 이동
   - 노드 데이터 쓰기

3. map.bin 작업
   - 파일 열기 (r+b 모드)
   - 없으면 새로 생성 및 초기화
   - 노드 인덱스 위치로 이동
   - offset 정보 업데이트

##### 파일 초기화
1. data.bin
   - 빈 파일로 생성
   - 필요한 시점에 데이터 기록

2. map.bin
   - 노드 수(256) 기록
   - 모든 offset 0으로 초기화
   - 총 크기: 4 + (256 * 8) bytes

##### 에러 처리
1. 디렉토리 생성 실패
   - 권한 문제
   - 경로 문제
   - 디스크 공간 부족

2. 파일 생성/열기 실패
   - 권한 문제
   - 파일 시스템 문제
   - 리소스 제한

3. 파일 작업 실패
   - Seek 실패
   - Write 실패
   - 디스크 공간 부족

##### 사용 예시
```c
// 노드 데이터 수정 후 저장
modify_node_data(node_index);
if (!save_node_to_file(node_index)) {
    // 실패 처리
    handle_error();
    return ERROR_CODE;
}
```

##### 주의사항
1. 원자성
   - data.bin 작업 실패 시 map.bin 작업 중단
   - 각 파일 작업의 독립성 유지
   - 실패 시 일관성 보장

2. 에러 처리
   - 모든 파일 작업 검증
   - 적절한 에러 메시지
   - 리소스 정리 보장

3. 성능 고려
   - 필요한 시점에만 호출
   - 파일 작업 최소화
   - 효율적인 버퍼 사용

##### 구현 세부사항
1. 파일 작업 순서
   ```c
   // 1. data.bin 작업
   FILE* data_file = fopen(DATA_FILE, "r+b");
   if (!data_file) return false;
   
   // 데이터 쓰기 작업...
   fclose(data_file);
   
   // 2. map.bin 작업
   FILE* map_file = fopen(MAP_FILE, "r+b");
   if (!map_file) return false;
   
   // offset 업데이트...
   fclose(map_file);
   ```

2. 크기 계산
   ```c
   // 노드 크기 계산 (2의 거듭제곱)
   size_t node_size = 1 << (*(ushort*)node);
   ```

3. Offset 계산
   ```c
   // map.bin에서 노드의 offset 위치
   long map_offset = sizeof(uint) + (node_index * sizeof(long));
   ``` 

### Save All Nodes
```c
bool save_all_nodes();
```

#### Purpose
Saves all nodes in memory to data.bin and updates their offsets in map.bin.

#### Process
1. File Initialization
   - Open data.bin in read/write mode
   - Create if doesn't exist
   - Open map.bin in read/write mode
   - Create if doesn't exist

2. Node Saving Loop
   - Iterate through all nodes
   - Write node data to data.bin
   - Update offset in map.bin
   - Handle errors appropriately

3. Resource Management
   - Proper file handle cleanup
   - Error state cleanup
   - Atomic operation (all or nothing)

#### Error Handling
1. File Operations
   - File open failures
   - Seek failures
   - Write failures

2. Node Validation
   - Invalid node positions
   - Missing nodes
   - Size mismatches

3. Resource Cleanup
   - Close files on any error
   - Close files after completion
   - Maintain file handle safety

#### Implementation Notes
1. File Safety
   - Files opened once at start
   - Closed on any error
   - Closed after completion

2. Error Recovery
   - Return false on any error
   - Close all files before return
   - Clear error messages

3. Performance
   - Single file open per operation
   - Minimized file operations
   - Efficient error handling

#### Usage Example
```c
if (!save_all_nodes()) {
    printf("Error: Failed to save nodes\n");
    // Handle error...
}
```

### node Creation

#### Command Interface
```shell
create-node
```

##### 기능
- 새로운 노드 생성
- 기본 구조 초기화
- 시스템 상태 업데이트

##### 프로세스
1. 초기화
   - 16 bytes 메모리 할당
   - 기본값으로 초기화
   - 기본 채널 설정

2. 시스템 업데이트
   - CurrentnodeCount 증가
   - Core 배열 업데이트
   - CoreMap 정보 설정

3. 파일 동기화
   - 노드 데이터 저장
   - 매핑 정보 업데이트
   - 오프셋 계산

##### 초기 구조
```
Offset  Content     Description
0-1     04 00      Size Power (2^4 = 16 bytes)
2-5     0E 00 00 00 Actual Size (14 bytes)
6-7     01 00      Channel Count (1)
8-11    0C 00 00 00 Channel 0 Offset (12)
12-13   00 00      Axis Count (0)
14-15   00 00      Reserved
```

##### 사용 예시
```shell
# 새 노드 생성
> create-node
Successfully created new node at index 5

# 최대 개수 초과 시
> create-node
Error: Maximum number of vertices (256) reached
```

##### 주의사항
1. 노드 제한
   - 최대 256개 노드
   - 인덱스 0-255
   - 제한 초과 시 에러

2. 메모리 관리
   - 적절한 메모리 할당
   - 리소스 정리
   - 메모리 누수 방지

3. 파일 동기화
   - 즉시 저장
   - 오프셋 계산
   - 매핑 정보 유지 

### node Deletion

#### Command Interface
```shell
delete-node <node_index>
```

##### 기능
- 노드 데이터 초기화
- 가비지 체인에 추가
- 메모리 관리 업데이트

##### 프로세스
1. 초기화
   - 노드 데이터를 기본값으로 리셋
   - 기본 채널 구조 유지
   - axis count 초기화

2. 가비지 체인 연결
   - 가비지 노드(0)의 첫 번째 링크 읽기
   - 삭제할 노드를 가비지 노드에 연결
   - 삭제할 노드를 이전 첫 번째 노드에 연결

3. 메모리 관리
   - Core 배열에서 제거
   - CoreMap 상태 업데이트
   - CoreSize 감소

##### 사용 예시
```shell
# 노드 삭제
> delete-node 5
Successfully deleted node 5

# 가비지 노드 삭제 시도
> delete-node 0
Error: Cannot delete garbage node (index 0)
```

##### 주의사항
1. 가비지 체인
   - 가비지 노드(0) 보호
   - 체인 일관성 유지
   - 순환 참조 방지

2. 메모리 관리
   - 메모리 누수 방지
   - CoreMap 상태 정확성
   - Core 배열 관리

3. 데이터 정리
   - 완전한 초기화
   - 링크 정리
   - 참조 제거 

## Memory Safety

### node Size Calculation
The system provides a safe way to calculate node sizes:

```c
static size_t get_node_size(const uchar* node)
```

#### Size Power Limits
The system limits node size power to 16 (64KB) for several reasons:

1. Memory Safety
   - Prevents integer overflow on 32-bit systems
   - Avoids undefined behavior with large shifts
   - Ensures consistent behavior across platforms

2. Resource Management
   - 64KB per node is sufficient for most uses
   - Prevents accidental allocation of huge nodes
   - Helps maintain reasonable memory usage

3. Error Detection
   - Size powers > 16 likely indicate corruption
   - Early detection of potential issues
   - Clear error reporting for debugging

#### Size Calculation Example
```c
// Maximum valid size: 2^16 = 65,536 bytes (64KB)
size_t max_valid_size = 1 << 16;

// Larger sizes are rejected:
if (size_power > 16) {
    // Error: Size too large
    return 0;
}
```

#### Implementation Benefits
1. Platform Independence
   - Works on both 32-bit and 64-bit systems
   - Avoids architecture-specific issues
   - Maintains consistent behavior

2. Error Prevention
   - Catches corrupted size values early
   - Prevents memory allocation issues
   - Simplifies debugging

3. Performance
   - Fast size validation
   - Simple power-of-2 calculations
   - Efficient memory management

#### Usage Example
```c
uchar* node = Core[node_position];
size_t size = get_node_size(node);
if (size == 0) {
    // Handle error...
}
```

#### Implementation Details
1. Size Power Reading
   - Uses memcpy for alignment-safe access
   - Reads exactly 2 bytes
   - Maintains platform independence

2. Validation
   - Maximum size power of 16 (64KB)
   - Returns 0 on invalid size
   - Clear error reporting

3. Calculation
   - Safe left shift operation
   - Proper type casting
   - Overflow prevention

[Rest of the document remains the same...]