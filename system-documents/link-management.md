# Link Management

## Overview
Link는 노드의 채널 간 연결을 나타내는 메커니즘입니다. 각 링크는 목적지 노드와 채널 정보를 포함하며, 특정 axis를 통해 연결됩니다.

## Memory Layout

### Offset Calculations
모든 메모리 접근은 다음 세 가지 오프셋의 조합으로 이루어집니다:

1. Channel Offset
   - 노드 시작점부터의 기본 오프셋
   - get_channel_offset() 함수로 계산
   - 모든 채널 데이터의 기준점

2. Axis Offset
   - 채널 오프셋 기준의 상대적 위치
   - get_axis_offset() 함수로 계산
   - 실제 사용시 channel_offset과 합산 필요

3. Link Data Offset
   - channel_offset + axis_offset: 링크 카운트 위치
   - channel_offset + axis_offset + 2: 첫 번째 링크 데이터 시작
   - 각 링크 엔트리: 6 bytes (node: 4, channel: 2)

### Memory Access Pattern
```c
// Link count access
ushort* link_count = (ushort*)(node + channel_offset + axis_offset);

// Link data access
Link* link_data = (Link*)(node + channel_offset + axis_offset + 2 + (index * 6));
```

## Link Creation Process

### 1. Offset 계산
```c
uint channel_offset = get_channel_offset(node, source_ch);
int axis_offset = get_axis_offset(node, source_ch, axis_number);
ushort* link_count = (ushort*)(node + channel_offset + axis_offset);
```

### 2. 공간 계산
```c
uint link_data_offset = channel_offset + axis_offset + 2 + (current_link_count * 6);
ushort required_size = link_data_offset + sizeof(Link);
```

### 3. 메모리 관리
- 필요시 노드 크기 확장
- 오프셋 재계산
- 링크 데이터 저장

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

#### 데이터 삽입 과정
1. 위치 검사
   ```c
   ushort channel_count = *(ushort*)(node + 2);
   bool is_last_channel = (source_ch == channel_count - 1);
   bool is_last_axis = (axis_offset == last_axis_offset);
   ```

2. 데이터 이동 계산
   ```c
   uint move_start = link_insert_offset;
   uint move_size = current_node_size - move_start;  // 남은 모든 데이터 이동
   ```
   - move_start: 새로운 링크가 삽입될 위치
   - move_size: 현재 노드 크기에서 삽입 위치를 뺀 크기
   - 장점:
     * 정확한 크기 계산 불필요
     * 모든 후속 데이터를 한 번에 이동
     * 경계 계산 오류 방지

3. 데이터 이동 실행
   ```c
   memmove(node + move_start + 6,
           node + move_start,
           move_size);
   ```

#### 주의사항
1. 채널 위치 확인
   - 마지막 채널인지 확인
   - 채널 오프셋 테이블 업데이트 필요

2. Axis 위치 확인
   - 채널 내 마지막 axis인지 확인
   - 후속 axis들의 offset 업데이트 필요

3. 데이터 이동 범위
   - 현재 위치부터 마지막 데이터까지
   - 채널 간 데이터 이동 고려

4. Offset 업데이트 순서
   - 현재 채널의 axis offsets
   - 후속 채널들의 channel offsets
   - 후속 채널들의 axis offsets

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

### Memory Space Calculation

#### Offset Types
1. Channel Offset
   - 채널의 시작 위치
   - 노드 시작점 기준

2. Axis Offset
   - 특정 axis의 데이터 위치
   - 채널 시작점 기준
   - Link count 위치를 가리킴

3. Last Axis Offset
   - 마지막 axis의 데이터 위치
   - 채널 시작점 기준
   - 새로운 데이터 추가 위치 계산에 사용

#### 필요 공간 계산 방법
1. 마지막 링크 위치 계산
   ```c
   uint last_axis_offset = get_last_axis_offset(node, source_ch);
   uint last_link_offset = channel_offset + last_axis_offset + 2 + (current_link_count * 6);
   ```
   - last_axis_offset: 마지막 axis의 위치
   - 2: link count 필드 크기
   - current_link_count * 6: 현재 저장된 링크들의 총 크기

2. 새로운 링크를 위한 공간
   ```c
   uint required_size = last_link_offset + 6;  // 새로운 링크를 위한 6바이트 추가
   ```

#### 메모리 접근
1. Link Count 읽기
   ```c
   ushort* link_count = (ushort*)(node + channel_offset + axis_offset);
   ```

2. Link 데이터 쓰기
   ```c
   // 마지막 위치에 새 링크 추가
   memcpy(node + last_link_offset, &link, sizeof(Link));
   ```

#### 주의사항
1. Offset 구분
   - axis_offset: 현재 작업중인 axis의 위치
   - last_axis_offset: 마지막 axis의 위치
   - 두 값은 다를 수 있음

2. 공간 계산
   - 항상 마지막 axis를 기준으로 계산
   - 새로운 데이터는 마지막 axis 다음에 추가

#### Offset 업데이트 규칙

1. 현재 채널 내 Axis Offset
   - 현재 axis 이후의 모든 axis offset 업데이트 필요
   - axis offset은 채널 시작점 기준의 상대 위치
   ```c
   // Update offsets in current channel
   for (int i = 0; i < axis_count; i++) {
       uint current_axis_offset = *(uint*)(node + axis_data_offset + (i * 6) + 2);
       if (current_axis_offset > axis_offset) {
           *(uint*)(node + axis_data_offset + (i * 6) + 2) += 6;
       }
   }
   ```

2. 후속 채널 Offset
   - 채널 offset만 업데이트
   - axis offset은 업데이트 불필요 (상대 위치이므로)
   ```c
   // Update only channel offsets
   if (!is_last_channel) {
       for (int ch = source_ch + 1; ch < channel_count; ch++) {
           uint* channel_offset_ptr = (uint*)(node + 4 + (ch * 4));
           *channel_offset_ptr += 6;
       }
   }
   ```

#### 주의사항
1. Offset 상대성
   - Channel offset: 노드 시작점 기준 (절대 위치)
   - Axis offset: 채널 시작점 기준 (상대 위치)
   - 데이터 이동 시 각각 적절히 처리 필요

2. 업데이트 범위
   - 현재 채널: axis offset 업데이트 필요
   - 후속 채널: channel offset만 업데이트
   - axis offset은 채널 기준 상대값이므로 불변

### Link Creation Process

#### 주의사항
1. 포인터 재계산
   - resize_node_space 호출 후 node 포인터가 변경될 수 있음
   - link_count 포인터 재계산 필요
   ```c
   // 올바른 순서
   memcpy(node + link_insert_offset, &link, sizeof(Link));
   ushort* link_count = (ushort*)(node + channel_offset + axis_offset);  // 포인터 재계산
   (*link_count)++;
   ```

2. 잘못된 구현
   ```c
   ushort* link_count = (ushort*)(node + channel_offset + axis_offset);
   // ... resize_node_space 호출 ...
   (*link_count)++;  // 오류: node가 변경되어 link_count가 잘못된 주소를 가리킴
   ```

3. 메모리 안전성
   - 포인터는 항상 최신 node 주소 기준으로 계산
   - resize 후 이전 포인터 사용 금지
   - 모든 오프셋 재계산 필요

## Testing

### Multi-Channel Link Test
```c
int test_multi_channel_links(uint node_index);
```

#### Purpose
여러 채널이 있는 상태에서 링크 생성과 삭제를 테스트합니다. 두 개의 채널에 각각 100개의 링크를 생성하고 삭제하여 메모리 관리와 데이터 일관성을 검증합니다.

#### Test Process
1. 초기 설정
   - 두 번째 채널 생성 (채널 1)
   - 각 채널에 axis 0 생성
   - 초기 link count 저장

2. 링크 생성 (각 채널 100개)
   - 채널 0에 순차적 링크 생성
   - 채널 1에 순차적 링크 생성
   - 각 단계마다 link count 검증
   - 생성된 링크 정보 저장

3. 링크 삭제 (역순)
   - 채널 1부터 역순으로 삭제
   - 채널 0 링크 역순 삭제
   - 각 단계마다 link count 검증

4. 최종 상태 검증
   - 각 채널의 최종 link count 확인
   - 초기 상태와 비교

#### 검증 항목
1. Channel Management
   - 채널 생성 성공
   - 채널 오프셋 정확성
   - 채널 간 독립성

2. Link Creation
   - 각 채널별 링크 생성 성공
   - Link count 순차적 증가
   - 채널 간 간섭 없음

3. Link Deletion
   - 각 채널별 링크 삭제 성공
   - Link count 순차적 감소
   - 채널 간 간섭 없음

4. Memory Management
   - 노드 크기 조정 정확성
   - 오프셋 업데이트 정확성
   - 메모리 누수 없음

#### 사용 예시
```c
// 노드 0에서 멀티채널 링크 테스트 실행
int failed = test_multi_channel_links(0);
if (failed > 0) {
    printf("%d tests failed\n", failed);
}
```

#### 주의사항
1. 채널 생성
   - 채널 0은 기본 존재
   - 채널 1 생성 실패 시 테스트 중단
   - 채널 간 독립성 보장

2. 메모리 관리
   - 노드 포인터 재로드 필수
   - 오프셋 재계산 필요
   - 크기 변경 추적

3. 에러 처리
   - 채널 생성 실패
   - 링크 생성/삭제 실패
   - count 불일치

[Rest of the document remains the same...]
 