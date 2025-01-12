# Node Management

## File Operations

### Node Data Persistence
노드 데이터의 파일 저장을 담당하는 중앙화된 함수를 제공합니다.

#### save_node_to_file
```c
bool save_node_to_file(unsigned int node_index);
```

##### 기능
- 노드 데이터를 data.bin 파일에 저장
- map.bin의 offset 정보 업데이트
- 파일 작업의 원자성 보장

##### 프로세스
1. 노드 유효성 검증
   - 인덱스 범위 검사
   - Core 배열 존재 여부 확인

2. data.bin 작업
   - 파일 열기 (r+b 모드)
   - CoreMap offset으로 이동
   - 노드 데이터 쓰기
   - 실패 시 즉시 반환

3. map.bin 작업
   - 파일 열기 (r+b 모드)
   - 노드 인덱스 위치로 이동
   - offset 정보 업데이트
   - 실패 시 즉시 반환

##### 에러 처리
1. 파일 작업 실패
   - 파일 열기 실패
   - Seek 실패
   - Write 실패
   - 각 단계별 적절한 정리

2. 에러 메시지
   - 구체적인 실패 원인 명시
   - 파일별 구분된 메시지
   - 작업 단계 식별 가능

3. 리소스 정리
   - 열린 파일 정리
   - 실패 시점 즉시 정리
   - 메모리 누수 방지

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

### Node Creation

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
   - CurrentNodeCount 증가
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
Error: Maximum number of nodes (256) reached
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