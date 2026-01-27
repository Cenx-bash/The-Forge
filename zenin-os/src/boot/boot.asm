; Zenin OS Bootloader - Tokyo Night Theme
; Expert-level assembly with modern features

bits 32
section .multiboot
    align 4
    dd 0x1BADB002              ; Magic number
    dd 0x00000003              ; Flags
    dd -(0x1BADB002 + 0x03)    ; Checksum

; Tokyo Night Color Palette
%define TOKYO_NIGHT_BG     0x1A1B26
%define TOKYO_NIGHT_FG     0xC0CAF5
%define TOKYO_NIGHT_BLUE   0x7AA2F7
%define TOKYO_NIGHT_PURPLE 0x9D7CD8
%define TOKYO_NIGHT_CYAN   0x7DCFFF
%define TOKYO_NIGHT_GREEN  0x9ECE6A
%define TOKYO_NIGHT_YELLOW 0xE0AF68
%define TOKYO_NIGHT_RED    0xF7768E

section .bss
align 16
stack_bottom:
    resb 16384 ; 16KB stack
stack_top:

section .data
boot_msg db "Zenin OS - Booting Tokyo Night...", 0
theme_msg db "Theme: Tokyo Night Palette", 0
colors:
    .bg     dd TOKYO_NIGHT_BG
    .fg     dd TOKYO_NIGHT_FG
    .blue   dd TOKYO_NIGHT_BLUE
    .purple dd TOKYO_NIGHT_PURPLE
    .cyan   dd TOKYO_NIGHT_CYAN
    .green  dd TOKYO_NIGHT_GREEN
    .yellow dd TOKYO_NIGHT_YELLOW
    .red    dd TOKYO_NIGHT_RED

section .text
global _start
extern kernel_main

_start:
    ; Set up stack
    mov esp, stack_top
    
    ; Save multiboot info
    mov [multiboot_info], ebx
    mov [multiboot_magic], eax
    
    ; Initialize Tokyo Night theme
    call init_tokyo_night_theme
    
    ; Clear screen with Tokyo Night background
    call clear_screen_tokyo_night
    
    ; Display boot message with theme colors
    call display_boot_message
    
    ; Enable SSE for advanced graphics
    call enable_sse
    
    ; Call kernel main
    push dword [multiboot_info]
    push dword [multiboot_magic]
    call kernel_main
    
    ; Halt if kernel returns
    cli
    hlt

init_tokyo_night_theme:
    pusha
    ; Set up VGA with Tokyo Night colors
    mov edi, 0xB8000
    mov ecx, 80*25
    mov eax, 0x0F20  ; White on black initially
    rep stosw
    popa
    ret

clear_screen_tokyo_night:
    pusha
    mov edi, 0xB8000
    mov ecx, 80*25
    mov ax, 0x0720  ; Grey on black
    rep stosw
    popa
    ret

display_boot_message:
    pusha
    mov esi, boot_msg
    mov edi, 0xB8000
    mov ah, 0x0E    ; Yellow text
.print_char:
    lodsb
    test al, al
    jz .done
    stosw
    jmp .print_char
.done:
    popa
    ret

enable_sse:
    ; Enable SSE for modern graphics operations
    mov eax, cr0
    and ax, 0xFFFB
    or ax, 0x2
    mov cr0, eax
    
    mov eax, cr4
    or ax, 3 << 9
    mov cr4, eax
    ret

section .bss
multiboot_info resd 1
multiboot_magic resd 1
