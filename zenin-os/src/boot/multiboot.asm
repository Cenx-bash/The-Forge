; Multiboot header for Zenin OS
; Ensures compatibility with GRUB and other bootloaders

section .multiboot
align 4

mb_header_start:
    dd 0x1BADB002              ; Magic number
    dd 0x00000007              ; Flags
    dd -(0x1BADB002 + 0x07)    ; Checksum
    
    ; AOUT kludge - not needed for ELF
    dd mb_header_start         ; header_addr
    dd 0x100000                ; load_addr
    dd 0                       ; load_end_addr
    dd 0                       ; bss_end_addr
    dd _start                  ; entry_addr

    ; Video mode
    dd 0                       ; linear graphics
    dd 1024                    ; width
    dd 768                     ; height
    dd 32                      ; depth
mb_header_end:
