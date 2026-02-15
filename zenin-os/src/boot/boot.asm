[BITS 32]
[ORG 0x7C00]

section .text
global _start

_start:
    ; Disable interrupts
    cli
    
    ; Set up stack
    mov esp, 0x9000
    mov ebp, esp
    
    ; Clear screen with Tokyo Night colors
    mov edi, 0xB8000
    mov ecx, 80*25
    mov ax, 0x1F20  ; Blue background, white text
.clear_screen:
    stosw
    loop .clear_screen
    
    ; Display boot message
    mov si, boot_msg
    call print_string
    
    ; Load kernel
    call load_kernel
    
    ; Set up GDT
    lgdt [gdt_descriptor]
    
    ; Switch to protected mode
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    
    ; Far jump to clear pipeline
    jmp CODE_SEG:protected_mode

print_string:
    pusha
    mov ah, 0x1F  ; Tokyo Night color scheme
.print_char:
    lodsb
    test al, al
    jz .done
    mov [edi], ax
    add edi, 2
    jmp .print_char
.done:
    popa
    ret

load_kernel:
    ; Kernel loading logic
    ret

; GDT Definition
gdt_start:
    dq 0x0
gdt_code:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0
gdt_data:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

protected_mode:
    ; Initialize segment registers
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Jump to kernel entry
    jmp 0x100000

boot_msg db "Zenin OS Booting... Tokyo Night Theme", 0

times 510-($-$$) db 0
dw 0xAA55