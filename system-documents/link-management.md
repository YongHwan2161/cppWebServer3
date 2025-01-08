# Link Management

## Overview
Link는 노드의 채널 간 연결을 나타내는 메커니즘입니다. 각 링크는 목적지 노드와 채널 정보를 포함하며, 특정 axis를 통해 연결됩니다.

## Data Structure

### Link Data Format (6 bytes)
```
[Node Index(4)] [Channel Index(2)]
```

### Memory Layout
```
[Link Count(2)] [Link 1(6)] [Link 2(6)] ... [Link N(6)]
```

### Memory Example
Axis with 2 links:
```
Offset    Content          Description
0000      02 00           Link count (2)
0002      01 00 00 00     Link 1: Node index (1)
0006      02 00           Link 1: Channel index (2)
0008      02 00 00 00     Link 2: Node index (2)
000C      01 00           Link 2: Channel index (1)
```

## Operations

### Link Creation
```c
int create_link(int source_node, int source_ch, 
               int dest_node, int dest_ch, 
               int axis_number);
```

#### Prerequisites
- 유효한 소스 노드와 채널
- Axis의 존재 (없을 경우 자동 생성)
- 충분한 메모리 공간

#### Process
1. 소스 노드/채널 유효성 검사
2. Axis 존재 여부 확인
   - 없으면 자동 생성
3. 현재 link count 확인
4. 필요한 공간 계산
   - Link 데이터 (6 bytes)
5. 공간 부족 시 메모리 재할당
   - Free Space 검색
   - 새로운 공간 할당
   - 데이터 복사
6. Link 데이터 저장
7. Link count 증가
8. 파일 동기화

### Link Deletion
```c
int delete_link(int source_node, int source_ch, 
               int dest_node, int dest_ch, 
               int axis_number);
```

#### Process Flow
1. 입력 유효성 검사
   - 소스 노드 존재 확인
   - 채널 오프셋 계산
   - Axis 존재 확인

2. 링크 데이터 접근
   - Axis 오프셋 획득
   - Link count 읽기
   - Link 데이터 영역 접근

3. 링크 검색
   - 모든 링크 순회
   - 대상 노드/채널 매칭
   - 위치 기록

4. 링크 삭제
   - 남은 링크 데이터 이동
   - Link count 감소
   - 메모리 정리

5. 파일 동기화
   - data.bin 업데이트
   - 변경사항 저장

#### Error Cases
- 유효하지 않은 소스 노드
- 존재하지 않는 Axis
- 찾을 수 없는 링크
- 파일 I/O 오류

#### Return Values
- LINK_SUCCESS: 링크 삭제 성공
- LINK_ERROR: 오류 발생

#### Example Usage
```c
// Delete forward link
int result = delete_link(0, 0,    // source node/channel
                        1, 0,     // destination node/channel
                        AXIS_FORWARD);  // axis number

if (result == LINK_SUCCESS) {
    printf("Link deleted successfully\n");
}
```

### Memory Management

#### Space Requirements
- Link count: 2 bytes
- Link 데이터: 6 bytes per link
  - Node Index: 4 bytes
  - Channel Index: 2 bytes

#### Node Resizing
공간 부족 시:
1. 새로운 크기 계산 (2의 제곱수)
2. Free Space 검색
3. 새로운 공간 할당
4. 기존 데이터 복사
5. 이전 공간 Free Space 반환

### File Synchronization
Link 생성 시 다음 파일들이 업데이트됩니다:

1. data.bin
   - 새로운 링크 데이터 저장
   - 노드 크기 변경 시 새로운 위치에 데이터 저장

2. map.bin
   - 노드 위치 변경 시 offset 정보 업데이트
   - 파일 구조: [노드 수(4)] [offset 1(8)] [offset 2(8)] ...

3. free_space.bin
   - 사용된/해제된 공간 정보 업데이트
   - 새로운 free block 추가

#### File Update Process
1. 노드 크기 변경 시:
   - 새로운 공간 할당
   - data.bin에 새 위치에 데이터 저장
   - map.bin의 offset 정보 업데이트
   - free_space.bin 업데이트

2. 링크 추가 시:
   - data.bin의 해당 위치에 링크 데이터 추가
   - 필요시 노드 크기 조정 및 관련 파일 업데이트

### Error Handling
파일 작업 관련 오류:
- data.bin 업데이트 실패: LINK_ERROR 반환
- map.bin 업데이트 실패: 경고 메시지 출력
- free_space.bin 업데이트 실패: 경고 메시지 출력

### Command Line Interface
링크 관리를 위한 CLI 명령어:

1. 링크 생성
```
create-link <src_node> <src_ch> <dst_node> <dst_ch> <axis>
```

2. 링크 삭제
```
delete-link <src_node> <src_ch> <dst_node> <dst_ch> <axis>
```

각 명령어는 다음을 검증합니다:
- 노드 인덱스 범위 (0-255)
- 채널 존재 여부
- Axis 유효성
- 링크 존재 여부 (삭제 시)

## Usage Example
```c
// Forward link 생성
create_link(0, 0,    // source node/channel
           1, 0,     // destination node/channel
           AXIS_FORWARD);  // axis number

// Backward link 생성
create_link(1, 0,    // source node/channel
           0, 0,     // destination node/channel
           AXIS_BACKWARD); // axis number
``` 