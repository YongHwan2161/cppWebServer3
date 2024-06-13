section .bss
    input resb 256             ; 입력을 저장할 256 바이트 버퍼
    length resd 1              ; 입력된 문자열의 길이를 저장
    option resb 2              ; 옵션 입력을 저장할 2 바이트 버퍼
    reversed resb 256          ; 반전된 문자열을 저장할 버퍼
    number resd 1              ; 팩토리얼을 계산할 숫자
    result resd 1              ; 팩토리얼 결과 저장
    result_string resb 12      ; 최대 12자리 숫자 저장 (예상 가능한 최대 팩토리얼 결과 길이)

section .data
    hello_message db '1.그대로, 2. 반전 3.시저암호 적용 4. 팩토리얼 5.exit',0xA  ; 메시지와 개행 문자
    hello_length equ $ - hello_message     ; 메시지의 길이 계산

section .text
    global _start

_start:
    ; compile code = 
    ; nasm -f elf32 hello.asm -o hello.o
    ; ld -m elf_i386 hello.o -o hello

    ; 시스템 호출을 위한 준비
    mov eax, 4              ; 'sys_write' 시스템 호출 번호
    mov ebx, 1              ; 파일 기술자 1 (표준 출력)
    mov ecx, hello_message  ; 메시지의 시작 주소
    mov edx, hello_length   ; 메시지의 길이
    int 0x80                ; 시스템 호출 실행

    ; 사용자로부터 옵션 입력 받기 (1: 그대로 출력, 2: 반전 출력)
    mov eax, 3                 ; 'sys_read' 시스템 호출 번호
    mov ebx, 0                 ; 파일 기술자 0 (표준 입력)
    mov ecx, option            ; 옵션 버퍼의 시작 주소
    mov edx, 2                 ; 버퍼 크기
    int 0x80                   ; 시스템 호출 실행

    ; 사용자로부터 입력 받기
    mov eax, 3                 ; 'sys_read' 시스템 호출 번호
    mov ebx, 0                 ; 파일 기술자 0 (표준 입력)
    mov ecx, input             ; 버퍼의 시작 주소
    mov edx, 256               ; 버퍼 크기
    int 0x80                   ; 시스템 호출 실행

    ; 입력받은 데이터의 길이 저장 후, 마지막 개행 문자 제거
    ; dec eax
    mov [length], eax

    ; 옵션 체크
    cmp byte [option], '1'
    je print_direct           ; 옵션 1일 경우, 직접 출력
    cmp byte [option], '2'
    je reverse_and_print      ; 옵션 2일 경우, 반전 후 출력
    cmp byte [option], '3'
    je caesar_cipher          ; 옵션 3일 경우, 시저 암호 적용
    cmp byte [option], '4'
    je calculate_factorial    ; 옵션 4일 경우, 팩토리얼 계산 
    cmp byte [option], '5'
    jmp exit    ; 옵션 4일 경우, 팩토리얼 계산 

print_direct:
    ; 입력받은 문자열 그대로 출력
    mov eax, 4
    mov ebx, 1
    mov ecx, input
    mov edx, [length]
    int 0x80
    jmp _start

reverse_and_print:
    ; 문자열 뒤집기
    lea esi, [input]           ; 원본 문자열 시작 주소
    lea edi, [reversed + 255]  ; 반전된 문자열 끝 주소
    mov ecx, [length]          ; 문자열 길이

reverse_loop:
    mov al, [esi]              ; 원본 문자열에서 바이트 읽기
    test al, al
    js multibyte_char          ; 다중 바이트 문자인지 확인

single_byte_char:
    mov [edi], al
    inc esi
    dec edi
    loop reverse_loop
    jmp print_reversed

multibyte_char:
    ; UTF-8 문자 시작 바이트 확인
    ; UTF-8 첫 바이트는 다중 바이트 문자의 길이를 결정
    mov al, [esi]
    test al, 0xE0              ; 0xE0와 AND 연산하여 1110 0000 확인
    jz two_byte_char
    test al, 0xF0              ; 0xF0와 AND 연산하여 1111 0000 확인
    jz three_byte_char
    test al, 0xF8              ; 0xF8와 AND 연산하여 1111 1000 확인
    jz four_byte_char

two_byte_char:
    ; 2바이트 문자 처리
    movsw
    loop reverse_loop
    jmp single_byte_char

three_byte_char:
    ; 3바이트 문자 처리
    movsb
    movsb
    movsb
    loop reverse_loop
    jmp single_byte_char

four_byte_char:
    ; 4바이트 문자 처리
    movsb
    movsb
    movsb
    movsb
    loop reverse_loop
    jmp single_byte_char

print_reversed:
    ; reversed + 256에서 [length]를 뺀 주소를 계산
    lea edx, [reversed + 256]
    sub edx, [length]

    ; 뒤집힌 문자열 출력
    mov eax, 4
    mov ebx, 1
    mov ecx, edx
    mov edx, [length]
    int 0x80
    jmp _start

caesar_cipher:
    ; 시저 암호 적용
    mov ecx, [length]
    mov ebx, input
caesar_loop:
    mov al, [ebx]
    cmp al, 'z'
    jle is_lower
    cmp al, 'Z'
    jle is_upper
    jmp next_char

is_lower:
    cmp al, 'z'
    je wrap_around_lower
    inc al
    jmp set_char

is_upper:
    cmp al, 'Z'
    je wrap_around_upper
    inc al
    jmp set_char

wrap_around_lower:
    mov al, 'a'
    jmp set_char

wrap_around_upper:
    mov al, 'A'

set_char:
    mov [ebx], al

next_char:
    inc ebx
    loop caesar_loop

    ; 암호화된 문자열 출력
    mov eax, 4
    mov ebx, 1
    mov ecx, input
    mov edx, [length]
    int 0x80
    jmp exit
calculate_factorial:
    ; 숫자 입력을 숫자로 변환
    mov ecx, [length]         ; 입력 길이
    lea ebx, [input]          ; 입력 시작 주소
    xor eax, eax              ; eax 초기화
convert_loop:
    mov dl, [ebx]             ; 현재 문자 가져오기
    sub dl, '0'               ; 문자에서 숫자로 변환
    imul eax, eax, 10         ; 기존 수를 10배
    add eax, edx              ; 현재 숫자 추가
    inc ebx                   ; 다음 문자로
    dec ecx                   ; ecx 감소
    jnz convert_loop          ; ecx가 0이 아니면 루프 반복

    ; 입력된 숫자 저장
    mov [number], eax

    ; 팩토리얼 계산
    mov eax, [number]         ; 계산할 숫자
    mov ecx, eax              ; 카운터 설정
    mov edx, 1                ; 결과 초기화
factorial_loop:
    mul ecx                   ; eax에 ecx 곱하기
    dec ecx                   ; ecx 감소
    jnz factorial_loop        ; ecx가 0이 아니면 루프 반복

    ; 결과 저장
    mov [result], eax

    ; 결과 출력
    mov eax, [result]         ; 팩토리얼 결과
    mov ecx, 10               ; 숫자를 문자열로 변환할 때 사용할 기준 (10진법)
    lea edi, [result_string + 11] ; 문자열 끝에서부터 작업 (null 종료 문자 포함)
    mov byte [edi], 0         ; null 종료 문자 설정
    dec edi                   ; null 바로 앞으로 이동

convert_to_string:
    xor edx, edx              ; edx 초기화 (div 명령어 사용 준비)
    div ecx                   ; eax를 10으로 나누어 몫은 eax에, 나머지는 edx에 저장
    add dl, '0'               ; 숫자를 해당하는 문자 코드로 변환
    mov [edi], dl             ; 현재 자리수 문자 저장
    dec edi                   ; 다음 자리수로 이동
    test eax, eax             ; eax가 0인지 확인
    jnz convert_to_string     ; 0이 아니면 계속 숫자 변환

    ; 문자열 출력 준비
    mov eax, 4                ; 'sys_write' 시스템 호출 번호
    mov ebx, 1                ; 파일 기술자 1 (표준 출력)
    lea ecx, [edi + 1]        ; 변환된 문자열의 시작 주소
    mov edx, result_string + 11
    sub edx, ecx
    int 0x80
    jmp exit

exit:
    ; 프로그램 종료
    mov eax, 1
    xor ebx, ebx
    int 0x80

