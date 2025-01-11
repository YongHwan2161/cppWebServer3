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
               unsigned int node_index, unsigned short channel_index,
               unsigned int move_size);
```

#### 기능
- Link entry (node index + channel)를 메모리에 삽입
- 6바이트 크기의 link entry 구조 처리
- 기존 데이터 자동 이동

#### Parameters
- dest: 대상 버퍼
- insert_pos: 삽입 위치
- node_index: 대상 노드 인덱스 (4 bytes)
- channel_index: 대상 채널 인덱스 (2 bytes)
- move_size: 이동해야 할 데이터 크기

#### Data Format
```
[Node Index(4)] [Channel Index(2)]
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
unsigned int node = 1;
unsigned short channel = 2;

// Insert link entry at position 0
int result = insert_link(buffer, 0, node, channel, 10);
// Result: buffer now contains link entry at start
```

#### Memory Layout
Link entry는 다음과 같은 메모리 레이아웃을 가집니다:
```
Offset    Content          Description
0000      01 00 00 00     Node index (4 bytes)
0004      02 00           Channel index (2 bytes)
```

#### Implementation Notes
1. 구조체 패킹
   ```c
   struct {
       unsigned int node;
       unsigned short channel;
   } __attribute__((packed)) link_data;
   ```
   - 메모리 정렬 최적화
   - 단일 memcpy 연산
   - 바이트 패딩 제거

2. 데이터 정렬
   - 4바이트 node index
   - 2바이트 channel index
   - 총 6바이트 크기

3. 성능 최적화
   - 구조체를 통한 단일 연산
   - 인라인 함수 활용
   - 최소 메모리 접근 