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