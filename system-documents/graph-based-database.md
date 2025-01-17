# Getting Started

## Building the Project
The project uses a standard Makefile build system. To build:

```bash
make
```

For detailed build instructions, see [Build Instructions](build-instructions.md).

## Running the Program
After building, run the program:

```bash
./cgdb
```

The program will automatically:
1. Create binary-data directory if needed
2. Initialize or load existing database
3. Set up memory management structures

# vertex
- vertex는 데이터를 구분하는 기본 단위이다. 
- vertex는 고유한 index가 0부터 순차적으로 부여된다. 
- 처음 database가 초기화 될 때에는 0부터 255까지의 index가 자동으로 생성된다. 각각의 vertex는 index 번호가 vertex에 저장된 1바이트의 데이터를 나타낼 수 있다.  
- 각 vertex의 정보는 바이트 단위로 binary file에 저장이 된다. 프로그램 실행중에는 필요한 vertex에 대한 index정보를 가리키는 별도의 테이블을 이용해서 필요한 노드 데이터를 저장장치(ssd or hdd)로부터 읽어들일 수 있다. 
- 즉 데이터를 저장하는 데 필요한 바이너리 파일은 두 종류이다. 하나는 각각의 vertex data가 저장된 `data.bin` file이고, 다른 하나는 index마다 해당 index의 vertex data가 저장된 binarary file사에서의 offset을 mapping해 주는 `map.bin`파일이다. 
- 데이터는 index 단위로 구분된다. 즉, 프로그램 실행시에는 vertex data들의 vector로 메모리에서 관리할 수 있다. 각 vector의 요소들은 바이트 단위의 배열(unsigned char[])이다. 
- 각 노드 data의 시작은 data size를 저장하는 4byte로 시작한다. data size는 자기 자신은 제외한다. 즉 실제 vertex data를 나타내는데 100 bytes가 필요하다면 vertex size에는 96이 기록된다. 이렇게 하는 이유는 데이터를 읽거나 쓸 때 먼저 vertex size 4 bytes를 읽고, 읽은 값만큼 바로 읽거나 쓰면 되기 때문이다. vertex size에 4를 빼지 않은 값을 저장하면 데이터를 저장장치에 저장하거나 읽을 때 4 bytes를 항상 빼줘야 하기 때문에 비효율적이다.
- data size 다음에는 channel 개수를 알려주는 2 bytes가 기록된다. 
- channel 개수 다음에는 각 채널의 offset을 가리키는 4 bytes * (채널 개수) 만큼의 데이터가 기록된다. 
- 채널 데이터에 접근하기 위해서는 채널 수 정보 다음부터 원하는 채널번호의 offset을 찾아서 해당 offset으로 이동하면 된다. offset은 vertex data의 시작점을 기준으로 계산한다. 

# vertex 구조
## 기본 구조
vertex의 기본 크기는 16 bytes (2^4)이며, 다음과 같은 구조를 가진다:
1. Allocated Size Power (2 bytes): 2의 지수로 표현된 할당된 노드 크기 (예: 4는 2^4=16 bytes를 의미)
2. Actual Used Size (4 bytes): 실제 사용중인 데이터 크기
3. Channel Count (2 bytes): 노드가 가진 채널의 수
4. Channel Offsets (4 bytes each): 각 채널의 오프셋
5. Channel Data: 각 채널의 실제 데이터

### 장점
1. 데이터 이동 최적화
   - 데이터 이동 시 정확한 크기 계산 가능
   - 복잡한 오프셋 계산 불필요
   - 한 번의 연산으로 이동 크기 결정

2. 구현 단순화
   - 마지막 데이터 위치 즉시 확인 가능
   - 채널/axis 순회 없이 크기 확인
   - 코드 복잡도 감소

### 단점과 해결방안
1. 추가 저장공간
   - 노드당 4바이트 추가 필요
   - 전체 데이터베이스 크기 소폭 증가
   - 장점 대비 무시할만한 수준

2. 크기 관리
   - 데이터 변경 시 actual size 업데이트 필요
   - 단순 대입으로 해결 가능
   - 복잡한 계산보다 효율적

### 메모리 레이아웃
```
[Allocated Size Power(2)][Actual Used Size(4)][Channel Count(2)][Channel Offsets(4*N)]...
```

### Actual Size 관리
1. 초기 크기
   - 14 bytes로 초기화 (size power + actual size 포함)
   - Size Power (2 bytes)
   - Actual Size (4 bytes)
   - Channel Count (2 bytes)
   - First Channel Offset (4 bytes)
   - Axis Count (2 bytes)

2. 장점
   - 데이터 섹션의 마지막 오프셋 = actual size
   - 별도의 계산 없이 데이터 범위 확인 가능
   - 메모리 이동 크기 계산 단순화

3. 크기 계산 예시
   ```c
   // 데이터 섹션의 마지막 오프셋
   uint last_offset = *(uint*)(vertex + 2);  // actual size 직접 사용

   // 이동해야 할 데이터 크기
   uint move_size = last_offset - insert_position;
   ```

4. 효율성
   - 오프셋 계산 오버헤드 감소
   - 데이터 이동 연산 단순화
   - 메모리 관리 용이

[Rest of the document remains the same...]

## cycle Detection
The system provides functionality to detect and analyze circular paths in the vertex graph:

### cycle Information
- cycles are detected by following links through vertices
- A cycle exists when a path leads back to a previously visited vertex/channel
- cycle information includes all vertices and channels in the cycle

### Implementation
1. Path Tracking
   - Maintains visited vertices list
   - Records vertex and channel information
   - Detects repeated visits

2. cycle Analysis
   - Counts vertices in cycle
   - Records path information
   - Provides detailed cycle data

3. Memory Management
   - Dynamic allocation for path tracking
   - Cleanup of temporary structures
   - Resource management for large cycles

### Usage
```c
// Check for cycle
bool has_cycle = validate_cycle(vertex_index, channel_index, axis_number);

// Get detailed cycle information
cycleInfo* info = get_cycle_info(vertex_index, channel_index, axis_number);
printf("cycle contains %d vertices\n", info->count);
free_cycle_info(info);
```

### cycle Data Structure
```c
typedef struct {
    uint* vertices;         // Array of vertices in cycle
    ushort* channels;    // Array of channels in cycle
    int count;          // Number of vertices in cycle
} cycleInfo;
```

This structure provides:
- Complete path information
- Memory-efficient storage
- Easy iteration through cycle vertices

## 파일 저장 구조
- data.bin 파일에서 각 노드는 2의 제곱수 크기로 저장된다
- 노드 크기는 첫 2바이트에 지수 형태로 저장된다 (예: 4는 16바이트를 의미)
- map.bin 파일은 각 노드의 offset을 저장하여 빠른 접근을 가능하게 한다

# Channel
- 채널은 노드마다 1개 이상 부여될 수 있는 개념이다. 같은 노드 내에서 서로 다른 채널을 가진 경우에는, vertex에 저장된 데이터는 공유하지만, 각각의 채널은 서로 독립적이다. 따라서 한 채널에서의 연결 관계들은 다른 채널의 연결 관계들과 완전히 독립적이다.
- 각 노드는 적어도 하나의 채널을 가진다. 

# Axis
- 각 채널은 다른 노드의 채널과 link로 연결되는데, 이 때 axis라는 개념이 이용된다. axis는 link의 속성을 구분해 주는 개념이다. 예를 들면 forward link와 backward link는 서로 다른 axis(axis 0와 axis 1)으로 구분된다. forward 및 backward 뿐만 아니라 더 많은 axis를 정의하여 사용할 수 있다. 
- axis 3를 time axis로 정의하면 각 채널마다 axis 3로 채널이 생성된 시각에 대한 정보(8 bytes)를 저장하는 데이터와 연결시킬 수 있고, 그럼, 채널의 생성 시각, 수정시각 등 시간에 대한 정보를 forward, backward link들과는 독립적으로 관리할 수 있다. 이 데이터는 시각화할 때 별개의 UI를 적용하여 화면에 표시할 수도 있다.
- 
# Database 생성
- 다음과 같이 `uchar**` 포인터를 사용하여 vertex에 대한 정보들을 저장한다. 
- `uchar*`는 byte array의 시작주소를 가리키는 pointer이다. 
```c
    uchar** Core;
```
- pointer를 사용하여 데이터가 저장된 메모리 위치만 참조하므로, vertex data의 size를 포인터가 가리키는 메모리 주소의 처음부분에 저장해야 한다. vertex size를 나타내기 위해 4바이트를 사용한다. 
- 채널 개수를 나타내기 위해 2바이트를 사용하고, 최소 채널이 1개 있어야 하므로, 채널 1개를 나타내기 위해 4바이트를 사용하고, 처음에는 채널0에 아무 연결 관계도 저장하지 않기 때문에, axis 개수로 2bytes 만 지정하면 된다. 총 12 bytes가 필요하며 다음과 같이 초기화하면 된다. 처음에는 256개의 vertex를 생성해야 한다. 
```c
    uchar initValues[12] = {8, 0, 0, 0, 1, 0, 10, 0, 0, 0, 0, 0};
    void create_new_vertex(int index) {
        uchar* newvertex = (uchar*)malloc(12 * sizeof(uchar));
        for (int i = 0; i < 12; ++i) {
            newvertex[i] = initValues[i];
        }
        Core[index] = newvertex;  // 생성된 vertex를 Core 배열의 해당 index 위치에 저장
    }
    void create_DB() {
        printf("Creating new database...\n");
        Core = (uchar**)malloc(MaxCoreSize * sizeof(uchar*));
        CoreMap = (vertexMapping*)malloc(256 * sizeof(vertexMapping));
        
        // Initialize CoreMap with default values
        for (int i = 0; i < 256; ++i) {
            CoreMap[i].core_position = i;
            CoreMap[i].is_loaded = 1;
            CoreMap[i].file_offset = 4 + (16 * i);  // 각 노드는 16바이트, 헤더 4바이트
        }
        
        // Read offsets from map.bin
        FILE* map_file = fopen(MAP_FILE, "rb");
        if (map_file) {
            fseek(map_file, sizeof(uint), SEEK_SET);
            for (int i = 0; i < 256; i++) {
                fread(&CoreMap[i].file_offset, sizeof(long), 1, map_file);
            }
            fclose(map_file);
        }
        
        // Create 256 vertices
        for (int i = 0; i < 256; ++i) {
            create_new_vertex(i);
            CoreSize++;
        }
    }
```

- Core 배열은 각각의 vertex에 대한 포인터를 저장하는 배열이다. 각 vertex의 index는 Core 배열의 index와 일치한다.
- create_new_vertex 함수는 vertex를 생성하고 이를 Core 배열의 해당 index 위치에 저장한다.
- create_DB 함수는 Core 배열을 할당하고 256개의 vertex를 생성하여 각각의 index 위치에 저장한다.

# Database 저장
- 데이터 베이스는 기본적으로 HDD 또는 SSD 등 영구적인 저장장치에 저장되어 있어야 하며, RAM에는 일시적으로 필요한 데이터만 올려서 사용해야 한다. 따라서 새로운 vertex를 생성하거나 기존의 vertex data를 수정할 때마다 데이터베이스가 저장된 binary file의 내용을 적절하게 수정해야 한다. 

## Binary Files 구조
1. data.bin
   - 실제 vertex data가 저장되는 파일
   - 각 vertex의 데이터가 연속적으로 저장됨
   - 각 vertex의 데이터는 size(4bytes) + 실제 데이터로 구성

2. map.bin
   - vertex index와 data.bin에서의 offset을 매핑하는 파일
   - 파일 시작에 전체 vertex 개수(4bytes) 저장
   - 각 vertex의 offset 정보가 index 순서대로 저장(각 8bytes)

## 저장 구현
```c
void save_vertex_to_file(FILE* data_file, FILE* map_file, int index) {
    uchar* vertex = Core[index];
    long offset = ftell(data_file);  // Get current position in data file
    
    // Write offset to map file
    fwrite(&offset, sizeof(long), 1, map_file);
    
    // Write vertex data to data file
    uint vertex_size = *(uint*)vertex;  // First 4 bytes contain size
    fwrite(vertex, sizeof(uchar), vertex_size + 4, data_file);  // +4 to include size field
}

void save_DB() {
    FILE* data_file = fopen("data.bin", "wb");
    FILE* map_file = fopen("map.bin", "wb");
    
    if (!data_file || !map_file) {
        printf("Error opening files for writing\n");
        return;
    }
    
    // Write number of vertices at the start of map file
    uint num_vertices = 256;
    fwrite(&num_vertices, sizeof(uint), 1, map_file);
    
    // Save each vertex
    for (int i = 0; i < 256; i++) {
        save_vertex_to_file(data_file, map_file, i);
    }
    
    fclose(data_file);
    fclose(map_file);
}
```

- save_vertex_to_file 함수는 개별 vertex를 binary file에 저장하는 함수이다:
  1. 현재 data file의 위치(offset)를 구한다
  2. 이 offset을 map file에 저장한다
  3. vertex의 실제 데이터를 data file에 저장한다

- save_DB 함수는 전체 데이터베이스를 저장하는 함수이다:
  1. data.bin과 map.bin 파일을 생성한다
  2. map.bin 파일의 시작에 전체 vertex 개수를 저장한다
  3. 각 vertex를 순차적으로 저장한다
  4. 저장이 완료되면 파일들을 닫는다

이러한 구조를 통해 프로그램이 다시 시작될 때 map.bin 파일을 읽어서 각 vertex의 위치를 빠르게 찾을 수 있으며, 필요한 vertex의 데이터만 data.bin 파일에서 읽어올 수 있다.

# Database 초기화
- 프로그램이 시작될 때 다음과 같은 순서로 데이터베이스를 초기화한다:
  1. binary-data 디렉토리에 data.bin과 map.bin 파일이 존재하는지 확인한다.
  2. 파일이 존재하지 않으면 새로운 데이터베이스를 생성하고 저장한다.
  3. 파일이 존재하면 기존 데이터베이스를 사용한다.

```c
int check_and_init_DB() {
    FILE* data_file = fopen(DATA_FILE, "rb");
    FILE* map_file = fopen(MAP_FILE, "rb");
    
    if (!data_file || !map_file) {
        if (data_file) fclose(data_file);
        if (map_file) fclose(map_file);
        
        // Files don't exist, create new database
        create_DB();
        save_DB();
        return 1;  // New database created
    }
    
    // TODO: Load existing database
    fclose(data_file);
    fclose(map_file);
    printf("Found existing database files\n");
    return 0;  // Existing database found
}
```

## 파일 구조
- binary-data/
  - data.bin: 실제 vertex 데이터가 저장된 파일
  - map.bin: vertex index와 data.bin의 offset 매핑 정보가 저장된 파일

# Database 로딩
- 기존 데이터베이스를 로딩하는 과정은 다음과 같다:
  1. map.bin 파일에서 전체 vertex 개수를 읽는다
  2. 각 vertex의 offset을 map.bin에서 순차적으로 읽는다
  3. offset을 이용하여 data.bin에서 실제 vertex 데이터를 읽어온다
  4. 읽어온 데이터를 메모리에 할당하고 Core 배열에 저장한다

## 로딩 구현
```c
void load_vertex_from_file(FILE* data_file, long offset, int index) {
    // Move to the correct position in data file
    fseek(data_file, offset, SEEK_SET);
    
    // Read size first
    uint vertex_size;
    fread(&vertex_size, sizeof(uint), 1, data_file);
    
    // Allocate memory for the vertex
    uchar* newvertex = (uchar*)malloc((vertex_size + 4) * sizeof(uchar));
    
    // Move back to read the whole vertex including size
    fseek(data_file, offset, SEEK_SET);
    fread(newvertex, sizeof(uchar), vertex_size + 4, data_file);
    
    // Store in Core
    Core[index] = newvertex;
}

void load_DB() {
    FILE* data_file = fopen(DATA_FILE, "rb");
    FILE* map_file = fopen(MAP_FILE, "rb");
    
    if (!data_file || !map_file) {
        printf("Error opening files for reading\n");
        return;
    }
    
    // Read number of vertices
    uint num_vertices;
    fread(&num_vertices, sizeof(uint), 1, map_file);
    
    // Allocate Core array
    Core = (uchar**)malloc(num_vertices * sizeof(uchar*));
    
    // Read each vertex's offset and load the vertex
    for (int i = 0; i < num_vertices; i++) {
        long offset;
        fread(&offset, sizeof(long), 1, map_file);
        load_vertex_from_file(data_file, offset, i);
    }
    
    fclose(data_file);
    fclose(map_file);
}
```

- load_vertex_from_file 함수는 개별 vertex를 binary file에서 읽어오는 함수이다:
  1. 주어진 offset 위치로 이동한다
  2. vertex size를 읽어온다
  3. 필요한 메모리를 할당한다
  4. 전체 vertex 데이터를 읽어온다
  5. Core 배열의 해당 index 위치에 저장한다

- load_DB 함수는 전체 데이터베이스를 로딩하는 함수이다:
  1. binary file들을 연다
  2. 전체 vertex 개수를 읽는다
  3. Core 배열을 할당한다
  4. 각 vertex의 offset을 읽고 해당 vertex를 로딩한다
  5. 파일들을 닫는다

이러한 구현을 통해 프로그램이 시작될 때 기존 데이터베이스를 효율적으로 메모리에 로딩할 수 있다.

# Core 메모리 관리
## Core 구조
- Core는 현재 RAM에 로드된 노드들의 포인터를 관리하는 배열
- 모든 노드를 RAM에 동시에 유지하지 않고, 필요한 노드만 로드
- 최대 MaxCoreSize(1024)개의 노드만 RAM에 유지

## CoreMap 구조
```c
typedef struct {
    int core_position;   // Position in Core array (-1 if not loaded)
    int is_loaded;      // 1 if loaded in RAM, 0 if not
    long file_offset;   // Offset position in data.bin
} vertexMapping;
```

### 구조체 설명
- core_position: Core 배열에서의 위치 (-1은 메모리에 없음을 의미)
- is_loaded: RAM 적재 상태 (1: 적재됨, 0: 적재되지 않음)
- file_offset: data.bin 파일에서의 offset 위치

### 초기화 과정
1. 메모리 할당
   - 256개의 노드에 대한 매핑 정보 저장 공간 할당
   - 기본값으로 초기화 (core_position: -1, is_loaded: 0)

2. Offset 정보 로드
   - 프로그램 시작 시 map.bin에서 모든 offset 정보를 읽어옴
   - CoreMap의 file_offset 필드에 저장
   - 이후 노드 접근 시 map.bin을 다시 읽을 필요 없음

### 메모리 최적화
- map.bin의 offset 정보를 미리 메모리에 로드
- 노드 데이터 접근 시 파일 I/O 최소화
- 빠른 노드 위치 탐색 가능

### 구현 예시
```c
void init_core_mapping() {
    CoreMap = (vertexMapping*)malloc(256 * sizeof(vertexMapping));
    
    // Initialize with default values
    for (int i = 0; i < 256; i++) {
        CoreMap[i].core_position = -1;
        CoreMap[i].is_loaded = 0;
        CoreMap[i].file_offset = 0;
    }
    
    // Read offsets from map.bin
    FILE* map_file = fopen(MAP_FILE, "rb");
    if (map_file) {
        fseek(map_file, sizeof(uint), SEEK_SET);
        for (int i = 0; i < 256; i++) {
            fread(&CoreMap[i].file_offset, sizeof(long), 1, map_file);
        }
        fclose(map_file);
    }
}
```

### 주요 기능
1. 노드 로딩 관리
   - Core 배열의 현재 상태 추적
   - 노드의 RAM 내 위치 매핑
   - 로드/언로드 상태 관리

2. 메모리 최적화
   - 필요한 노드만 RAM에 유지
   - MaxCoreSize 초과 시 자동 언로드
   - 메모리 사용량 제어

### 구현된 함수들
```c
void init_core_mapping(void);        // CoreMap 초기화
int get_core_position(int vertex_index);  // Core에서의 위치 조회
int load_vertex_to_core(int vertex_index);  // 노드를 Core에 로드
void unload_vertex_from_core(int vertex_index);  // 노드를 Core에서 언로드
```

### 동작 방식
1. 노드 접근 시
   - CoreMap을 통해 노드의 현재 상태 확인
   - RAM에 없으면 자동으로 로드
   - MaxCoreSize 초과 시 다른 노드 언로드

2. 메모리 관리
   - 노드 로드 시 여유 공간 확인
   - 공간 부족 시 사용 빈도가 낮은 노드 언로드
   - Core 배열 재정렬 및 매핑 정보 업데이트

## vertex Validation
노드 접근 시 항상 validate_vertex() 함수를 통해 유효성을 검증해야 합니다.

### 검증 항목
1. 노드 인덱스 범위 (0-255)
2. Core 배열에서의 노드 존재 여부

### 호출 책임
함수를 호출하는 쪽에서 반드시 인자의 유효성을 먼저 검증해야 합니다. 이는 다음과 같은 이유 때문입니다:

1. 책임 분리
   - 각 함수는 자신의 고유 기능에만 집중
   - 유효성 검증은 호출하는 쪽의 책임
   - 함수 코드의 복잡도 감소

2. 코드 효율성
   - 불필요한 유효성 검증 코드 중복 방지
   - 함수 내부 코드 간소화
   - 유지보수 용이성 증가

### 올바른 사용 예시
```c
// 잘못된 방식
create_axis(vertex_index, channel_index, axis_number);  // 유효성 검증을 함수에 떠넘김

// 올바른 방식
if (!validate_vertex(vertex_index)) {
    return ERROR_CODE;
}
// 노드가 유효한 경우에만 함수 호출
create_axis(vertex_index, channel_index, axis_number);
```

### 함수 설계 원칙
1. 인자 검증
   - 함수 호출 전 모든 인자의 유효성 검증
   - validate_vertex()로 노드 유효성 확인
   - 채널, axis 등 다른 인자들도 검증

2. 에러 처리
   - 유효성 검증 실패 시 즉시 반환
   - 명확한 에러 메시지 출력
   - 적절한 에러 코드 반환

3. 코드 구조
```c
int some_function(uint vertex_index, ushort channel_index) {
    // 1. 노드 유효성 검증
    if (!validate_vertex(vertex_index)) {
        return ERROR_CODE;
    }
    
    // 2. 채널 유효성 검증
    if (!validate_channel(vertex_index, channel_index)) {
        return ERROR_CODE;
    }
    
    // 3. 모든 검증이 통과된 후에만 실제 작업 수행
    // ...
}
```

### 장점
1. 코드 품질
   - 명확한 책임 구분
   - 중복 코드 감소
   - 가독성 향상

2. 유지보수
   - 검증 로직 중앙화
   - 버그 발생 가능성 감소
   - 코드 수정 용이

3. 성능
   - 불필요한 함수 호출 방지
   - 조기 에러 감지
   - 리소스 낭비 방지

### 주의사항
1. 검증 순서
   - 가장 기본적인 검증부터 시작
   - 의존성 있는 검증은 후순위로
   - 비용이 적은 검증을 먼저 수행

2. 에러 메시지
   - 구체적인 실패 원인 명시
   - 문제 해결을 위한 힌트 제공
   - 일관된 메시지 형식 유지

3. 성능 고려
   - 필요한 검증만 수행
   - 중복 검증 방지
   - 검증 비용 최소화
```

### Garbage Management

#### Garbage cycle Detection
The system provides functionality to detect and protect vertices in the garbage cycle:

1. Detection Function
```c
bool is_in_garbage_cycle(uint vertex_index);
```
- Checks if a vertex is part of garbage cycle
- Starts from garbage vertex (0)
- Follows garbage chain links

2. Protection Mechanism
- Prevents deletion of vertices in garbage cycle
- Maintains garbage chain integrity
- Ensures data consistency

3. Usage in Vertex Deletion
```c
// Check before deletion
if (is_in_garbage_cycle(vertex_index)) {
    return VERTEX_ERROR_IN_GARBAGE_cycle;
}
```

#### Benefits
1. Data Integrity
   - Prevents corruption of garbage chain
   - Maintains valid deletion states
   - Protects system stability

2. Error Prevention
   - Early detection of invalid deletions
   - Clear error messages
   - Proper error handling

3. System Reliability
   - Consistent garbage management
   - Protected garbage cycle
   - Safe vertex deletion

## cycle Management

### cycle Creation
The system provides functionality to create cycles by connecting a series of vertices:

```c
int create_cycle(uint* vertices, ushort* channels, int count, ushort axis_number);
```

#### Parameters
- vertices: Array of vertex indices to connect
- channels: Array of channel indices corresponding to vertices
- count: Number of vertices/channels in the cycle
- axis_number: Axis to use for connections

#### Process
1. Input Validation
   - Check for valid arrays
   - Minimum 2 vertices required
   - Arrays must match in size

2. Link Creation
   - Connect consecutive vertices
   - Create final link to close cycle
   - Use specified axis for all links

3. Error Handling
   - Return LINK_ERROR if any link fails
   - Return LINK_SUCCESS on completion
   - Validate all parameters

#### Usage Example
```c
uint vertices[] = {1, 2, 3};
ushort channels[] = {0, 0, 0};
int count = 3;

// Create cycle using forward links (axis 0)
int result = create_cycle(vertices, channels, count, 0);
if (result == LINK_SUCCESS) {
    printf("Successfully created cycle\n");
}
```

#### Notes
1. cycle Structure
   - Each vertex connects to next in array
   - Last vertex connects to first
   - All links use same axis

2. Memory Management
   - No dynamic allocation
   - Arrays must be pre-allocated
   - Caller manages memory

3. Error Cases
   - NULL array pointers
   - Count less than 2
   - Link creation failure
