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
1. 유효성 검사
   - 가비지 노드(0) 삭제 방지
   - 가비지 순환 체크
   - 삭제 가능 여부 확인

2. 초기화
   - 노드 데이터를 기본값으로 리셋
   - 기본 채널 구조 유지
   - axis count 초기화

3. 가비지 체인 연결
   - 가비지 노드(0)의 첫 번째 링크 읽기
   - 삭제할 노드를 가비지 노드에 연결
   - 삭제할 노드를 이전 첫 번째 노드에 연결

##### 사용 예시
```shell
# 노드 삭제
> delete-vertex 5
Successfully deleted vertex 5

# 가비지 노드 삭제 시도
> delete-vertex 0
Error: Cannot delete garbage vertex (index 0)

# 가비지 순환에 포함된 노드 삭제 시도
> delete-vertex 3
Error: Cannot delete vertex in garbage cycle
```

##### 주의사항
1. 가비지 체인
   - 가비지 노드(0) 보호
   - 순환 감지 및 방지
   - 체인 일관성 유지

2. 순환 검사
   - 삭제 전 순환 체크
   - 가비지 순환 보호
   - 데이터 일관성 유지 