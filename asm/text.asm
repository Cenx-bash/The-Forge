section .data
    text db 'Hello, Assembly World!', 0
    target db 'a', 0
    count dw 0

section .text
    global _start

_start:
    mov rsi, text       ; source string
    mov rcx, 0          ; counter
    
count_chars:
    lodsb               ; load byte into al
    cmp al, 0           ; end of string?
    je done_count
    inc rcx
    jmp count_chars

done_count:
    ; rcx contains string length
    
    ; Exit with length as status
    mov rax, 60
    mov rdi, rcx
    syscall
