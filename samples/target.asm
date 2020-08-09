SECTION .BSS
i: resd 0
x: resd 0
SECTION .DATA
y: DW 1
data0: db 'done',10
data0len: equ $-data0
message db "%08x", 10, 0

SECTION .TEXT
GLOBAL main
extern printf

main:
mov eax,4
mov ebx,1
mov ecx,data0
mov edx,data0len
int 80h

xor eax,eax
mov eax,[y]
add eax,1

push eax
push message
call printf
add esp,8

mov eax,1
mov ebx,0
int 80h
