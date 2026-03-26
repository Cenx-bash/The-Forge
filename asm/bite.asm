section .text
    global _start

_start:
    ; Bit manipulation examples
    mov al, 0xFF        ; 11111111
    mov bl, 0x0F        ; 00001111
    
    ; Set bit 3
    bts ax, 3
    
    ; Clear bit 5
    btr ax, 5
    
    ; Test bit 4
    bt ax, 4
    
    ; Rotate operations
    rol al, 1           ; rotate left
    ror al, 1           ; rotate right
    
    ; Exit
    mov eax, 1
    xor ebx, ebx
    int 0x80
