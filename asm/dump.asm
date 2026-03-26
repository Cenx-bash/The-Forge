section .data
    hex_chars db '0123456789ABCDEF'
    newline db 10

section .bss
    buffer resb 16

section .text
    global _start

_start:
    ; Example: dump memory region
    mov rsi, buffer     ; source address
    mov rcx, 16         ; bytes to dump
    
dump_loop:
    push rcx
    mov al, [rsi]
    call print_byte
    mov al, ' '
    call print_char
    pop rcx
    inc rsi
    loop dump_loop
    
    call print_newline
    
    ; Exit
    mov rax, 60
    xor rdi, rdi
    syscall

print_byte:
    push rax
    shr al, 4
    call print_nibble
    pop rax
    and al, 0x0F
    call print_nibble
    ret

print_nibble:
    mov rbx, hex_chars
    xlat
    call print_char
    ret

print_char:
    push rax
    mov [buffer], al
    mov rax, 1
    mov rdi, 1
    mov rsi, buffer
    mov rdx, 1
    syscall
    pop rax
    ret

print_newline:
    mov al, 10
    call print_char
    ret
