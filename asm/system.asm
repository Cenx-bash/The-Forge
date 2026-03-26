section .data
    msg db 'System call example', 10
    len equ $ - msg

section .text
    global _start

_start:
    ; Write to stdout
    mov rax, 1          ; sys_write
    mov rdi, 1          ; stdout
    mov rsi, msg        ; message
    mov rdx, len        ; length
    syscall
    
    ; Get current time
    mov rax, 201        ; sys_time
    xor rdi, rdi        ; NULL
    syscall
    ; time is now in rax
    
    ; Exit with time as status
    mov rdi, rax        ; exit code
    mov rax, 60         ; sys_exit
    syscall
