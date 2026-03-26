section .data
    registers times 16 dw 0
    pc dw 0              ; program counter
    running db 1

section .text
    global _start

_start:
    ; Simple CPU simulation loop
sim_loop:
    cmp byte [running], 0
    je done
    
    ; Fetch instruction
    mov bx, [pc]
    ; Decode and execute would go here
    ; ... simulation logic ...
    
    inc word [pc]        ; increment PC
    jmp sim_loop

done:
    ; Exit
    mov eax, 1
    xor ebx, ebx
    int 0x80
