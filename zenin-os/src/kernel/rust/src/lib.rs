#![no_std]
#![feature(allocator_api)]
#![feature(lang_items)]

extern crate alloc;

use core::alloc::Layout;
use core::panic::PanicInfo;

mod syscalls;
mod memory;

// Tokyo Night color constants
pub mod colors {
    pub const BACKGROUND: u32 = 0x1A1B26;
    pub const FOREGROUND: u32 = 0xA9B1D6;
    pub const BLUE: u32 = 0x7AA2F7;
    pub const PURPLE: u32 = 0xBB9AF7;
    pub const CYAN: u32 = 0x7DCFFF;
    pub const GREEN: u32 = 0x9ECE6A;
    pub const YELLOW: u32 = 0xE0AF68;
    pub const RED: u32 = 0xF7768E;
}

// Safe wrapper for kernel operations
pub struct Kernel {
    terminal: Terminal,
    memory: MemoryManager,
}

impl Kernel {
    pub unsafe fn new() -> Self {
        Kernel {
            terminal: Terminal::new(),
            memory: MemoryManager::new(),
        }
    }
    
    pub fn print(&self, text: &str) {
        self.terminal.write(text);
    }
    
    pub fn print_colored(&self, text: &str, color: u32) {
        self.terminal.set_color(color);
        self.print(text);
        self.terminal.reset_color();
    }
    
    pub fn banner(&self) {
        self.print_colored("╔══════════════════════════════════════╗\n", colors::BLUE);
        self.print_colored("║         Zenin OS - Tokyo Night       ║\n", colors::CYAN);
        self.print_colored("║           Professional Edition       ║\n", colors::PURPLE);
        self.print_colored("╚══════════════════════════════════════╝\n", colors::BLUE);
    }
}

// Memory-safe allocator
#[global_allocator]
static ALLOCATOR: memory::KernelAllocator = memory::KernelAllocator;

// Panic handler for Rust code
#[panic_handler]
fn panic(info: &PanicInfo) -> ! {
    unsafe {
        let kernel = Kernel::new();
        kernel.print_colored("[RUST PANIC] ", colors::RED);
        if let Some(location) = info.location() {
            kernel.print(&format!("at {}:{}:{} ", 
                location.file(), 
                location.line(), 
                location.column()));
        }
        if let Some(message) = info.message() {
            kernel.print(&format!("{}", message));
        }
        kernel.print("\n");
    }
    
    loop {}
}

// Language items required for no_std
#[lang = "eh_personality"]
extern "C" fn eh_personality() {}

#[no_mangle]
pub extern "C" fn _Unwind_Resume() -> ! {
    loop {}
}