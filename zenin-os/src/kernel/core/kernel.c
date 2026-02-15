#include <kernel/types.h>
#include <kernel/terminal.h>
#include <kernel/memory.h>
#include <kernel/panic.h>
#include <kernel/theme.h>

// Tokyo Night color palette
#define TN_BACKGROUND    0x1A1B26
#define TN_FOREGROUND    0xA9B1D6
#define TN_BLUE          0x7AA2F7
#define TN_PURPLE        0xBB9AF7
#define TN_CYAN          0x7DCFFF
#define TN_GREEN         0x9ECE6A
#define TN_YELLOW        0xE0AF68
#define TN_RED           0xF7768E

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint8_t bpp;
    uint8_t* framebuffer;
} video_mode_t;

static video_mode_t video_mode;
static terminal_t terminal;

void kernel_main(multiboot_info_t* mb_info) {
    // Initialize terminal with Tokyo Night theme
    terminal_initialize(TN_BACKGROUND, TN_FOREGROUND);
    
    // Print Zenin OS banner
    terminal_setcolor(TN_BLUE, TN_BACKGROUND);
    terminal_writestring("Zenin OS - Tokyo Night Edition\n");
    terminal_writestring("==============================\n\n");
    
    terminal_setcolor(TN_CYAN, TN_BACKGROUND);
    terminal_writestring("Kernel Version: 1.0.0\n");
    terminal_writestring("Architecture: x86_64\n");
    terminal_writestring("Build: Professional Edition\n\n");
    
    // Initialize memory manager
    terminal_setcolor(TN_GREEN, TN_BACKGROUND);
    terminal_writestring("[+] Initializing Memory Manager...\n");
    mm_init(mb_info);
    
    // Initialize interrupts
    terminal_writestring("[+] Setting up Interrupts...\n");
    idt_init();
    
    // Initialize drivers
    terminal_writestring("[+] Loading Drivers...\n");
    drivers_init();
    
    // Initialize filesystem
    terminal_writestring("[+] Mounting Filesystem...\n");
    vfs_init();
    
    // Initialize GUI
    terminal_writestring("[+] Starting Tokyo Night GUI...\n");
    gui_init();
    
    terminal_setcolor(TN_YELLOW, TN_BACKGROUND);
    terminal_writestring("\nSystem ready. Welcome to Zenin OS!\n");
    
    // Enter main loop
    kernel_idle();
}

void kernel_idle() {
    terminal_setcolor(TN_FOREGROUND, TN_BACKGROUND);
    terminal_writestring("\nzenin@localhost:~$ ");
    
    while(1) {
        // Handle input and system calls
        asm volatile("hlt");
    }
}

void panic(const char* message) {
    terminal_setcolor(TN_RED, TN_BACKGROUND);
    terminal_writestring("\n!!! KERNEL PANIC !!!\n");
    terminal_writestring(message);
    terminal_writestring("\nSystem halted.\n");
    
    while(1) {
        asm volatile("cli");
        asm volatile("hlt");
    }
}