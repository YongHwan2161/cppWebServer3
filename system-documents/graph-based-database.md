# Node
- Node는 데이터를 구분하는 기본 단위이다. 
- Node는 고유한 index가 0부터 순차적으로 부여된다. 
- 처음 database가 초기화 될 때에는 0부터 255까지의 index가 자동으로 생성된다. 각각의 node는 index 번호가 node에 저장된 1바이트의 데이터를 나타낼 수 있다.  
- 각 node의 정보는 바이트 단위로 binary file에 저장이 된다. 프로그램 실행중에는 필요한 node에 대한 index정보를 가리키는 별도의 테이블을 이용해서 필요한 노드 데이터를 저장장치(ssd or hdd)로부터 읽어들일 수 있다. 
- 즉 데이터를 저장하는 데 필요한 바이너리 파일은 두 종류이다. 하나는 각각의 node data가 저장된 `data.bin` file이고, 다른 하나는 index마다 해당 index의 node data가 저장된 binarary file사에서의 offset을 mapping해 주는 `map.bin`파일이다. 
- 데이터는 index 단위로 구분된다. 즉, 프로그램 실행시에는 node data들의 vector로 메모리에서 관리할 수 있다. 각 vector의 요소들은 바이트 단위의 배열(unsigned char[])이다. 
- 각 노드 data의 시작은 data size를 저장하는 4byte로 시작한다. data size는 자기 자신은 제외한다. 즉 실제 node data를 나타내는데 100 bytes가 필요하다면 node size에는 96이 기록된다. 이렇게 하는 이유는 데이터를 읽거나 쓸 때 먼저 node size 4 bytes를 읽고, 읽은 값만큼 바로 읽거나 쓰면 되기 때문이다. node size에 4를 빼지 않은 값을 저장하면 데이터를 저장장치에 저장하거나 읽을 때 4 bytes를 항상 빼줘야 하기 때문에 비효율적이다.
- data size 다음에는 channel 개수를 알려주는 2 bytes가 기록된다. 
- channel 개수 다음에는 각 채널의 offset을 가리키는 4 bytes * (채널 개수) 만큼의 데이터가 기록된다. 
- 채널 데이터에 접근하기 위해서는 채널 수 정보 다음부터 원하는 채널번호의 offset을 찾아서 해당 offset으로 이동하면 된다. offset은 node data의 시작점을 기준으로 계산한다. 

# Channel
- 채널은 노드마다 1개 이상 부여될 수 있는 개념이다. 같은 노드 내에서 서로 다른 채널을 가진 경우에는, Node에 저장된 데이터는 공유하지만, 각각의 채널은 서로 독립적이다. 따라서 한 채널에서의 연결 관계들은 다른 채널의 연결 관계들과 완전히 독립적이다.
- 각 노드는 적어도 하나의 채널을 가진다. 

# Axis
- 각 채널은 다른 노드의 채널과 link로 연결되는데, 이 때 axis라는 개념이 이용된다. axis는 link의 속성을 구분해 주는 개념이다. 예를 들면 forward link와 backward link는 서로 다른 axis(axis 0와 axis 1)으로 구분된다. forward 및 backward 뿐만 아니라 더 많은 axis를 정의하여 사용할 수 있다. 
- axis 3를 time axis로 정의하면 각 채널마다 axis 3로 채널이 생성된 시각에 대한 정보(8 bytes)를 저장하는 데이터와 연결시킬 수 있고, 그럼, 채널의 생성 시각, 수정시각 등 시간에 대한 정보를 forward, backward link들과는 독립적으로 관리할 수 있다. 이 데이터는 시각화할 때 별개의 UI를 적용하여 화면에 표시할 수도 있다.
- 
# Database 생성
- 다음과 같이 `uchar**` 포인터를 사용하여 node에 대한 정보들을 저장한다. 
- `uchar*`는 byte array의 시작주소를 가리키는 pointer이다. 
```c
    uchar** Core;
```
- pointer를 사용하여 데이터가 저장된 메모리 위치만 참조하므로, node data의 size를 포인터가 가리키는 메모리 주소의 처음부분에 저장해야 한다. node size를 나타내기 위해 4바이트를 사용한다. 
- 채널 개수를 나타내기 위해 2바이트를 사용하고, 최소 채널이 1개 있어야 하므로, 채널 1개를 나타내기 위해 4바이트를 사용하고, 처음에는 채널0에 아무 연결 관계도 저장하지 않기 때문에, axis 개수로 2bytes 만 지정하면 된다. 총 12 bytes가 필요하며 다음과 같이 초기화하면 된다. 처음에는 256개의 node를 생성해야 한다. 
```c
    uchar initValues[12] = {8, 0, 0, 0, 1, 0, 10, 0, 0, 0, 0, 0};
    void create_new_node(int index) {
        uchar* newNode = (uchar*)malloc(12 * sizeof(uchar));
        for (int i = 0; i < 12; ++i) {
            newNode[i] = initValues[i];
        }
        Core[index] = newNode;  // 생성된 node를 Core 배열의 해당 index 위치에 저장
    }
    void create_DB() {
        printf("call create_DB()\n");
        Core = (uchar**)malloc(256 * sizeof(uchar*));
        for (int i = 0; i < 256; ++i) {
            create_new_node(i);  // index를 전달하여 node 생성 및 저장
        }
    }
```

- Core 배열은 각각의 node에 대한 포인터를 저장하는 배열이다. 각 node의 index는 Core 배열의 index와 일치한다.
- create_new_node 함수는 node를 생성하고 이를 Core 배열의 해당 index 위치에 저장한다.
- create_DB 함수는 Core 배열을 할당하고 256개의 node를 생성하여 각각의 index 위치에 저장한다.

# Database 저장
- 데이터 베이스는 기본적으로 HDD 또는 SSD 등 영구적인 저장장치에 저장되어 있어야 하며, RAM에는 일시적으로 필요한 데이터만 올려서 사용해야 한다. 따라서 새로운 Node를 생성하거나 기존의 node data를 수정할 때마다 데이터베이스가 저장된 binary file의 내용을 적절하게 수정해야 한다. 

## Binary Files 구조
1. data.bin
   - 실제 node data가 저장되는 파일
   - 각 node의 데이터가 연속적으로 저장됨
   - 각 node의 데이터는 size(4bytes) + 실제 데이터로 구성

2. map.bin
   - node index와 data.bin에서의 offset을 매핑하는 파일
   - 파일 시작에 전체 node 개수(4bytes) 저장
   - 각 node의 offset 정보가 index 순서대로 저장(각 8bytes)

## 저장 구현
```c
void save_node_to_file(FILE* data_file, FILE* map_file, int index) {
    uchar* node = Core[index];
    long offset = ftell(data_file);  // Get current position in data file
    
    // Write offset to map file
    fwrite(&offset, sizeof(long), 1, map_file);
    
    // Write node data to data file
    uint node_size = *(uint*)node;  // First 4 bytes contain size
    fwrite(node, sizeof(uchar), node_size + 4, data_file);  // +4 to include size field
}

void save_DB() {
    FILE* data_file = fopen("data.bin", "wb");
    FILE* map_file = fopen("map.bin", "wb");
    
    if (!data_file || !map_file) {
        printf("Error opening files for writing\n");
        return;
    }
    
    // Write number of nodes at the start of map file
    uint num_nodes = 256;
    fwrite(&num_nodes, sizeof(uint), 1, map_file);
    
    // Save each node
    for (int i = 0; i < 256; i++) {
        save_node_to_file(data_file, map_file, i);
    }
    
    fclose(data_file);
    fclose(map_file);
}
```

- save_node_to_file 함수는 개별 node를 binary file에 저장하는 함수이다:
  1. 현재 data file의 위치(offset)를 구한다
  2. 이 offset을 map file에 저장한다
  3. node의 실제 데이터를 data file에 저장한다

- save_DB 함수는 전체 데이터베이스를 저장하는 함수이다:
  1. data.bin과 map.bin 파일을 생성한다
  2. map.bin 파일의 시작에 전체 node 개수를 저장한다
  3. 각 node를 순차적으로 저장한다
  4. 저장이 완료되면 파일들을 닫는다

이러한 구조를 통해 프로그램이 다시 시작될 때 map.bin 파일을 읽어서 각 node의 위치를 빠르게 찾을 수 있으며, 필요한 node의 데이터만 data.bin 파일에서 읽어올 수 있다.

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
  - data.bin: 실제 node 데이터가 저장된 파일
  - map.bin: node index와 data.bin의 offset 매핑 정보가 저장된 파일

# Database 로딩
- 기존 데이터베이스를 로딩하는 과정은 다음과 같다:
  1. map.bin 파일에서 전체 node 개수를 읽는다
  2. 각 node의 offset을 map.bin에서 순차적으로 읽는다
  3. offset을 이용하여 data.bin에서 실제 node 데이터를 읽어온다
  4. 읽어온 데이터를 메모리에 할당하고 Core 배열에 저장한다

## 로딩 구현
```c
void load_node_from_file(FILE* data_file, long offset, int index) {
    // Move to the correct position in data file
    fseek(data_file, offset, SEEK_SET);
    
    // Read size first
    uint node_size;
    fread(&node_size, sizeof(uint), 1, data_file);
    
    // Allocate memory for the node
    uchar* newNode = (uchar*)malloc((node_size + 4) * sizeof(uchar));
    
    // Move back to read the whole node including size
    fseek(data_file, offset, SEEK_SET);
    fread(newNode, sizeof(uchar), node_size + 4, data_file);
    
    // Store in Core
    Core[index] = newNode;
}

void load_DB() {
    FILE* data_file = fopen(DATA_FILE, "rb");
    FILE* map_file = fopen(MAP_FILE, "rb");
    
    if (!data_file || !map_file) {
        printf("Error opening files for reading\n");
        return;
    }
    
    // Read number of nodes
    uint num_nodes;
    fread(&num_nodes, sizeof(uint), 1, map_file);
    
    // Allocate Core array
    Core = (uchar**)malloc(num_nodes * sizeof(uchar*));
    
    // Read each node's offset and load the node
    for (int i = 0; i < num_nodes; i++) {
        long offset;
        fread(&offset, sizeof(long), 1, map_file);
        load_node_from_file(data_file, offset, i);
    }
    
    fclose(data_file);
    fclose(map_file);
}
```

- load_node_from_file 함수는 개별 node를 binary file에서 읽어오는 함수이다:
  1. 주어진 offset 위치로 이동한다
  2. node size를 읽어온다
  3. 필요한 메모리를 할당한다
  4. 전체 node 데이터를 읽어온다
  5. Core 배열의 해당 index 위치에 저장한다

- load_DB 함수는 전체 데이터베이스를 로딩하는 함수이다:
  1. binary file들을 연다
  2. 전체 node 개수를 읽는다
  3. Core 배열을 할당한다
  4. 각 node의 offset을 읽고 해당 node를 로딩한다
  5. 파일들을 닫는다

이러한 구현을 통해 프로그램이 시작될 때 기존 데이터베이스를 효율적으로 메모리에 로딩할 수 있다.