section .data
    filename db 'output.txt', 0
    msg db 'Hello, World!', 10
    msg_len equ $ - msg

section .bss
    fd resb 1

section .text
    global _start

_start:
    ; Create or open file
    mov rax, 2          ; sys_open
    mov rdi, filename   ; filename
    mov rsi, 0x42       ; O_CREAT|O_WRONLY
    mov rdx, 0644       ; permissions
    syscall
    
    ; Write to file
    mov rdi, rax        ; file descriptor
    mov rax, 1          ; sys_write
    mov rsi, msg        ; message
    mov rdx, msg_len    ; length
    syscall
    
    ; Close file
    mov rax, 3          ; sys_close
    syscall
    
    ; Exit
    mov rax, 60         ; sys_exit
    xor rdi, rdi
    syscall
