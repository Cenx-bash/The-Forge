section .text
    global _start

_start:
    ; Binary operations example
    mov al, 0b10101010  ; binary literal
    mov bl, 0b11001100
    
    and al, bl          ; AND operation
    or al, bl           ; OR operation
    xor al, bl          ; XOR operation
    
    ; Exit
    mov eax, 1
    xor ebx, ebx
    int 0x80
