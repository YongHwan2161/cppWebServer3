# vertex Management

## File Operations

### vertex Data Persistence
노드 데이터의 파일 저장을 담당하는 중앙화된 함수를 제공합니다.

#### save_vertex_to_file
```c
bool save_vertex_to_file(unsigned int vertex_index);
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
modify_vertex_data(vertex_index);
if (!save_vertex_to_file(vertex_index)) {
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
   size_t vertex_size = 1 << (*(ushort*)vertex);
   ```

3. Offset 계산
   ```c
   // map.bin에서 노드의 offset 위치
   long map_offset = sizeof(uint) + (vertex_index * sizeof(long));
   ``` 

### vertex Creation

#### Command Interface
```shell
create-vertex
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
   - CurrentvertexCount 증가
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
> create-vertex
Successfully created new vertex at index 5

# 최대 개수 초과 시
> create-vertex
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

### vertex Deletion

#### Command Interface
```shell
delete-vertex <vertex_index>
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
> delete-vertex 5
Successfully deleted vertex 5

# 가비지 노드 삭제 시도
> delete-vertex 0
Error: Cannot delete garbage vertex (index 0)
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