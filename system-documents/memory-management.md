# Memory Management

## Overview
메모리 관리 시스템은 데이터의 삽입, 이동, 복사 등을 안전하게 수행하는 기능을 제공합니다.

## Memory Operations

### Data Insertion
```c
int insert_memory(unsigned char* dest, unsigned int insert_pos,
                 const unsigned char* src, unsigned int src_size,
                 unsigned int total_size, unsigned int move_size);
```

#### 기능
- 메모리 버퍼의 특정 위치에 데이터를 삽입
- 기존 데이터를 자동으로 뒤로 이동
- 메모리 경계 검사 수행

#### Parameters
- dest: 대상 버퍼
- insert_pos: 삽입 위치
- src: 삽입할 데이터
- src_size: 삽입할 데이터 크기
- total_size: 대상 버퍼의 전체 크기
- move_size: 이동해야 할 데이터 크기

#### Process
1. 매개변수 검증
   - NULL 포인터 체크
   - 메모리 범위 검사

2. 데이터 이동
   - memmove로 기존 데이터를 뒤로 이동
   - 삽입 공간 확보

3. 데이터 삽입
   - memcpy로 새 데이터 복사
   - 삽입 위치에 데이터 배치

#### Return Values
- 1: 성공
- 0: 실패 (잘못된 매개변수 또는 메모리 범위 초과)

#### Usage Example
```c
unsigned char buffer[100] = {1, 2, 3, 4, 5};
unsigned char new_data[] = {10, 11, 12};

// Insert new_data at position 2
int result = insert_memory(buffer, 2, new_data, 3, 100, 3);
// Result: {1, 2, 10, 11, 12, 3, 4, 5}
```

### Memory Safety
1. 경계 검사
   - 삽입 위치 유효성 검증
   - 버퍼 오버플로우 방지
   - 전체 크기 확인

2. 데이터 이동
   - memmove 사용으로 오버랩 처리
   - 순차적 데이터 이동
   - 데이터 무결성 보장

3. 오류 처리
   - NULL 포인터 검사
   - 크기 범위 검증
   - 실패 시 즉시 반환

### Implementation Notes

#### Code Organization
1. Helper Functions
   ```c
   static inline void move_data_forward(unsigned char* dest, unsigned int pos, 
                                      unsigned int size, unsigned int move_size);
   ```
   - 공통 데이터 이동 로직 추출
   - 인라인 함수로 성능 최적화
   - 코드 중복 제거

2. Structured Data Access
   ```c
   struct {
       unsigned short number;
       unsigned int offset;
   } __attribute__((packed)) axis_data;
   ```
   - 패킹된 구조체로 데이터 정렬
   - 단일 memcpy 연산으로 효율성 향상
   - 바이트 정렬 보장

#### 최적화 전략

1. 메모리 접근
   - memcpy 사용으로 정렬 문제 해결
   - 구조체를 통한 단일 연산 수행
   - 캐시 효율성 향상

2. 함수 인라인화
   - 공통 로직의 인라인 처리
   - 함수 호출 오버헤드 감소
   - 컴파일러 최적화 지원

3. 데이터 정렬
   - packed 속성으로 불필요한 패딩 제거
   - 메모리 레이아웃 최적화
   - 접근 효율성 향상

#### 성능 개선사항

1. 데이터 이동
   - 단일 memmove 호출
   - 중복 코드 제거
   - 유지보수성 향상

2. 데이터 삽입
   - 구조체 기반 단일 연산
   - 타입 안전성 보장
   - 메모리 접근 최적화

3. 에러 처리
   - 최소한의 검증
   - 빠른 실패 처리
   - 오버헤드 감소

#### 메모리 안전성

1. 구조체 패킹
   ```c
   __attribute__((packed))
   ```
   - 바이트 정렬 강제
   - 플랫폼 독립성 보장
   - 메모리 레이아웃 제어

2. 데이터 복사
   - memcpy로 안전한 복사
   - 크기 기반 검증
   - 타입 안전성 유지

3. 경계 검사
   - NULL 포인터 검증
   - 범위 검사 (필요시)
   - 메모리 오버플로우 방지

#### 사용 예시

1. 최적화된 Axis Entry 삽입
```c
// 구조체를 통한 단일 연산 삽입
struct axis_entry {
    unsigned short number;
    unsigned int offset;
} __attribute__((packed)) entry = {1, 0x1000};

insert_axis_entry(buffer, 0, entry.number, entry.offset, 10);
```

2. 기본 타입 삽입
```c
// 효율적인 기본 타입 삽입
insert_ushort(buffer, 0, 0x1234, 10);
insert_uint(buffer, 2, 0x12345678, 8);
```

#### 성능 고려사항

1. 캐시 효율성
   - 연속된 메모리 접근
   - 단일 연산으로 데이터 처리
   - 캐시 라인 활용 최적화

2. 연산 최소화
   - 불필요한 함수 호출 제거
   - 중복 연산 방지
   - 메모리 접근 최소화

3. 컴파일러 최적화
   - 인라인 함수 활용
   - 구조체 패킹
   - 정적 함수 사용

### Axis Entry Insertion
```c
int insert_axis_entry(unsigned char* dest, unsigned int insert_pos,
                     unsigned short axis_number, unsigned int axis_offset,
                     unsigned int total_size, unsigned int move_size);
```

#### 기능
- Axis entry (axis number + offset)를 메모리에 삽입
- 6바이트 크기의 axis entry 구조 처리
- 기존 데이터 자동 이동

#### Parameters
- dest: 대상 버퍼
- insert_pos: 삽입 위치
- axis_number: 삽입할 axis 번호 (2 bytes)
- axis_offset: axis의 offset 값 (4 bytes)
- total_size: 대상 버퍼의 전체 크기
- move_size: 이동해야 할 데이터 크기

#### Data Format
```
[Axis Number(2)] [Axis Offset(4)]
```

#### Process
1. 매개변수 검증
   - NULL 포인터 체크
   - 메모리 범위 검사 (6바이트 + 이동 크기)

2. 데이터 이동
   - memmove로 기존 데이터를 6바이트 뒤로 이동
   - axis entry를 위한 공간 확보

3. 데이터 삽입
   - axis number 삽입 (2바이트)
   - axis offset 삽입 (4바이트)

#### Return Values
- 1: 성공
- 0: 실패 (잘못된 매개변수 또는 메모리 범위 초과)

#### Usage Example
```c
unsigned char buffer[100];
unsigned short axis_num = 1;
unsigned int axis_off = 0x1000;

// Insert axis entry at position 0
int result = insert_axis_entry(buffer, 0, axis_num, axis_off, 100, 0);
// Result: buffer now contains axis entry at start
```

### Memory Layout
Axis entry는 다음과 같은 메모리 레이아웃을 가집니다:
```
Offset    Content          Description
0000      01 00           Axis number (2 bytes)
0002      00 10 00 00     Axis offset (4 bytes)
```

### Basic Type Insertion

#### Unsigned Short Insertion
```c
int insert_ushort(unsigned char* dest, unsigned int insert_pos,
                 unsigned short value, unsigned int move_size);
```

##### 기능
- 2바이트 unsigned short 값을 메모리에 삽입
- 기존 데이터 자동 이동
- 리틀 엔디안 방식으로 저장

##### Parameters
- dest: 대상 버퍼
- insert_pos: 삽입 위치
- value: 삽입할 값
- move_size: 이동해야 할 데이터 크기

##### Process
1. 매개변수 검증
   - NULL 포인터 체크

2. 데이터 이동
   - memmove로 기존 데이터를 2바이트 뒤로 이동
   - 삽입 공간 확보

3. 데이터 삽입
   - unsigned short 값을 직접 메모리에 쓰기

#### Unsigned Int Insertion
```c
int insert_uint(unsigned char* dest, unsigned int insert_pos,
               unsigned int value, unsigned int move_size);
```

##### 기능
- 4바이트 unsigned int 값을 메모리에 삽입
- 기존 데이터 자동 이동
- 리틀 엔디안 방식으로 저장

##### Parameters
- dest: 대상 버퍼
- insert_pos: 삽입 위치
- value: 삽입할 값
- move_size: 이동해야 할 데이터 크기

##### Process
1. 매개변수 검증
   - NULL 포인터 체크

2. 데이터 이동
   - memmove로 기존 데이터를 4바이트 뒤로 이동
   - 삽입 공간 확보

3. 데이터 삽입
   - unsigned int 값을 직접 메모리에 쓰기

#### Usage Example
```c
unsigned char buffer[100];

// Insert unsigned short
unsigned short us_value = 0x1234;
insert_ushort(buffer, 0, us_value, 10);  // Moves 10 bytes forward

// Insert unsigned int
unsigned int ui_value = 0x12345678;
insert_uint(buffer, 2, ui_value, 8);     // Moves 8 bytes forward
```

### Memory Layout Examples

#### Unsigned Short
```
Before: [AA BB CC DD]
Insert 0x1234 at position 0:
After:  [34 12 AA BB CC DD]
        ^^^^^
        Inserted value (little-endian)
```

#### Unsigned Int
```
Before: [AA BB CC DD EE]
Insert 0x12345678 at position 0:
After:  [78 56 34 12 AA BB CC DD EE]
        ^^^^^^^^^
        Inserted value (little-endian)
```

### Implementation Notes

#### 데이터 정렬
1. Unsigned Short
   - 2바이트 크기
   - 리틀 엔디안 정렬
   - 메모리 경계 고려

2. Unsigned Int
   - 4바이트 크기
   - 리틀 엔디안 정렬
   - 메모리 경계 고려

#### 성능 최적화
1. 직접 메모리 접근
   - 포인터 캐스팅으로 직접 쓰기
   - 중간 버퍼 불필요
   - 최소 메모리 연산

2. 데이터 이동
   - memmove로 한 번에 이동
   - 오버랩 안전하게 처리
   - 불필요한 복사 방지

#### 주의사항
1. 엔디안 처리
   - 리틀 엔디안 시스템 가정
   - 필요시 바이트 순서 변환
   - 플랫폼 호환성 고려

2. 메모리 정렬
   - 정렬 경계 고려
   - 플랫폼별 최적화
   - 접근 효율성 확보 

### Link Entry Insertion
```c
int insert_link(unsigned char* dest, unsigned int insert_pos,
               unsigned int vertex_index, unsigned short channel_index,
               unsigned int move_size);
```

#### 기능
- Link entry (vertex index + channel)를 메모리에 삽입
- 6바이트 크기의 link entry 구조 처리
- 기존 데이터 자동 이동

#### Parameters
- dest: 대상 버퍼
- insert_pos: 삽입 위치
- vertex_index: 대상 노드 인덱스 (4 bytes)
- channel_index: 대상 채널 인덱스 (2 bytes)
- move_size: 이동해야 할 데이터 크기

#### Data Format
```
[vertex Index(4)] [Channel Index(2)]
```

#### Process
1. 매개변수 검증
   - NULL 포인터 체크

2. 데이터 이동
   - memmove로 기존 데이터를 6바이트 뒤로 이동
   - link entry를 위한 공간 확보

3. 데이터 삽입
   - 구조체를 통한 단일 연산으로 데이터 삽입
   - 패킹된 구조체로 메모리 정렬 보장

#### Return Values
- 1: 성공
- 0: 실패 (잘못된 매개변수)

#### Usage Example
```c
unsigned char buffer[100];
unsigned int vertex = 1;
unsigned short channel = 2;

// Insert link entry at position 0
int result = insert_link(buffer, 0, vertex, channel, 10);
// Result: buffer now contains link entry at start
```

#### Memory Layout
Link entry는 다음과 같은 메모리 레이아웃을 가집니다:
```
Offset    Content          Description
0000      01 00 00 00     vertex index (4 bytes)
0004      02 00           Channel index (2 bytes)
```

#### Implementation Notes
1. 구조체 패킹
   ```c
   struct {
       unsigned int vertex;
       unsigned short channel;
   } __attribute__((packed)) link_data;
   ```
   - 메모리 정렬 최적화
   - 단일 memcpy 연산
   - 바이트 패딩 제거

2. 데이터 정렬
   - 4바이트 vertex index
   - 2바이트 channel index
   - 총 6바이트 크기

3. 성능 최적화
   - 구조체를 통한 단일 연산
   - 인라인 함수 활용
   - 최소 메모리 접근 

## vertex Data Management

### vertex Unloading
```c
int unload_vertex_data(uint vertex_index);
```

#### Purpose
메모리에서 자주 사용되지 않는 노드 데이터를 해제합니다. 데이터는 binary file에 저장되어 있으므로, 필요할 때 다시 로드할 수 있습니다.

#### Parameters
- vertex_index: 언로드할 노드의 인덱스 (0-255)

#### Returns
- 1: 성공
- 0: 실패 (잘못된 인덱스 또는 이미 언로드됨)

#### Process
1. 유효성 검사
   - 노드 인덱스 범위 확인 (0-255)
   - 노드가 메모리에 로드되어 있는지 확인

2. 메모리 해제
   - Core[vertex_index]의 메모리 해제
   - Core[vertex_index]를 NULL로 설정

3. CoreMap 업데이트
   - is_loaded 플래그를 0으로 설정
   - core_position을 -1로 설정
   - CoreSize 감소

#### Usage Example
```c
// Unload vertex 5 from memory
if (unload_vertex_data(5)) {
    printf("Successfully unloaded vertex 5\n");
} else {
    printf("Failed to unload vertex 5\n");
}
```

#### Error Cases
1. 잘못된 노드 인덱스
   ```c
   if (unload_vertex_data(256)) {  // Invalid index
       // This will fail
   }
   ```

2. 이미 언로드된 노드
   ```c
   unload_vertex_data(5);  // First unload
   if (unload_vertex_data(5)) {  // Second unload
       // This will fail
   }
   ```

### Memory Management Strategy

#### Unload Criteria
노드 언로드 결정 시 고려사항:
1. 사용 빈도
   - 최근 접근 시간
   - 접근 횟수

2. 메모리 압박
   - 전체 로드된 노드 수
   - 사용 가능한 메모리

3. 데이터 상태
   - 수정된 데이터 여부
   - 저장 필요성

#### Implementation Notes
1. 메모리 관리
   - 포인터 정리
   - 메모리 누수 방지
   - NULL 체크

2. CoreMap 동기화
   - 상태 플래그 업데이트
   - 위치 정보 관리
   - 일관성 유지

3. 에러 처리
   - 잘못된 인덱스
   - 이중 해제 방지
   - 상태 검증 

### vertex Unloading Command

#### Command Interface
```shell
unload-vertex <vertex_index>
```

##### 기능
- 메모리에서 노드 데이터 해제
- CoreMap 상태 업데이트
- Core 배열 정리

##### 프로세스
1. 입력 검증
   - 노드 인덱스 범위 확인
   - 로드 상태 확인
   - 매개변수 유효성 검사

2. 메모리 해제
   - Core 배열 포인터 해제
   - NULL로 포인터 설정
   - CoreSize 감소

3. 상태 업데이트
   - CoreMap is_loaded 플래그 갱신
   - core_position 재설정
   - 메모리 관리 정보 갱신

##### 사용 예시
```shell
# 정상 케이스
> unload-vertex 5
Successfully unloaded vertex 5 from memory

# 에러 케이스
> unload-vertex 256
Error: vertex index must be between 0 and 255

> unload-vertex 5
Error: vertex 5 is not loaded in memory
```

##### 주의사항
1. 데이터 접근
   - 언로드 후 직접 접근 불가
   - 필요시 재로드 필요
   - Core 배열 인덱스 변경 가능

2. 메모리 관리
   - 자동 재로드 고려
   - 메모리 단편화 관리
   - 효율적인 메모리 사용

3. 상태 추적
   - CoreMap 상태 확인
   - 로드/언로드 이력 관리
   - 메모리 사용량 모니터링 

### vertex Loading Command

#### Command Interface
```shell
load-vertex <vertex_index>
```

##### 기능
- 파일에서 노드 데이터 로드
- Core 배열에 메모리 할당
- CoreMap 상태 업데이트

##### 프로세스
1. 입력 검증
   - 노드 인덱스 범위 확인
   - 이미 로드된 상태 확인
   - 매개변수 유효성 검사

2. 메모리 관리
   - CoreSize 확인
   - 필요시 다른 노드 언로드
   - 새 메모리 할당

3. 데이터 로드
   - 파일에서 데이터 읽기
   - Core 배열에 저장
   - CoreMap 업데이트

##### 사용 예시
```shell
# 정상 케이스
> load-vertex 5
Successfully loaded vertex 5 to Core position 3

# 에러 케이스
> load-vertex 256
Error: vertex index must be between 0 and 255

> load-vertex 5
vertex 5 is already loaded at Core position 3
```

##### 주의사항
1. 메모리 관리
   - MaxCoreSize 제한 확인
   - 자동 언로드 처리
   - 메모리 할당 실패 처리

2. 파일 접근
   - 파일 열기/닫기 관리
   - 오프셋 정확성 확인
   - I/O 에러 처리

3. 상태 동기화
   - CoreMap 상태 일관성
   - Core 배열 정확성
   - 로드 상태 추적 

### Core Size Monitoring

#### Command Interface
```shell
check-core-size
```

##### 기능
- Core 메모리 상태 확인
- 사용량 통계 표시
- 가용 공간 모니터링

##### 출력 정보
1. 크기 정보
   - Current Core Size: 현재 로드된 노드 수
   - Maximum Core Size: 최대 로드 가능 개수
   - Available Slots: 잔여 슬롯 수

2. 사용률 정보
   - Memory Utilization: 메모리 사용 비율
   - 백분율로 표시
   - 소수점 첫째 자리까지 표시

##### 사용 예시
```shell
> check-core-size
Core Memory Status:
Current Core Size: 3
Maximum Core Size: 16
Available Slots: 13
Memory Utilization: 18.8%
```

##### 활용 방안
1. 메모리 관리
   - 로드 전 가용 공간 확인
   - 언로드 필요성 판단
   - 최적화 시점 결정

2. 성능 모니터링
   - 메모리 부족 예방
   - 시스템 부하 관리
   - 리소스 계획 수립

3. 운영 최적화
   - 적정 사용률 유지
   - 불필요한 로드 방지
   - 효율적 메모리 관리

##### 주의사항
1. 사용률 관리
   - 높은 사용률 주의
   - 여유 공간 확보
   - 주기적 모니터링

2. 메모리 계획
   - 피크 사용량 예측
   - 버퍼 공간 유지
   - 적절한 언로드 정책 