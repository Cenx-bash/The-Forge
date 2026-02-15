#include <kernel/memory.h>
#include <kernel/panic.h>
#include <lib/bitmap.h>

#define MEMORY_BITMAP_SIZE 0x10000
#define PAGE_SIZE 4096

static bitmap_t memory_bitmap;
static uintptr_t memory_start;
static uintptr_t memory_end;
static uint32_t total_pages;
static uint32_t free_pages;

void pmm_init(multiboot_info_t* mb_info) {
    // Parse memory map from multiboot
    multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)mb_info->mmap_addr;
    
    memory_start = 0x100000; // Start after 1MB
    memory_end = 0;
    
    // Find available memory regions
    while((uint32_t)mmap < mb_info->mmap_addr + mb_info->mmap_length) {
        if(mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
            uintptr_t start = mmap->addr;
            uintptr_t end = start + mmap->len;
            
            if(start < memory_start) memory_start = start;
            if(end > memory_end) memory_end = end;
        }
        mmap = (multiboot_memory_map_t*)((uint32_t)mmap + mmap->size + 4);
    }
    
    // Initialize bitmap
    total_pages = (memory_end - memory_start) / PAGE_SIZE;
    memory_bitmap = bitmap_create(MEMORY_BITMAP_SIZE);
    
    // Mark all pages as free initially
    for(uint32_t i = 0; i < total_pages; i++) {
        bitmap_set(memory_bitmap, i, 0);
    }
    
    free_pages = total_pages;
    
    // Reserve kernel pages
    reserve_pages(0x100000, (0x200000 - 0x100000) / PAGE_SIZE);
}

void* pmm_alloc_page() {
    for(uint32_t i = 0; i < total_pages; i++) {
        if(!bitmap_test(memory_bitmap, i)) {
            bitmap_set(memory_bitmap, i, 1);
            free_pages--;
            return (void*)(memory_start + i * PAGE_SIZE);
        }
    }
    panic("Out of memory!");
    return NULL;
}

void pmm_free_page(void* page) {
    uintptr_t addr = (uintptr_t)page;
    if(addr < memory_start || addr >= memory_end) {
        panic("Invalid page address to free!");
    }
    
    uint32_t index = (addr - memory_start) / PAGE_SIZE;
    bitmap_set(memory_bitmap, index, 0);
    free_pages++;
}

void reserve_pages(uintptr_t start, uint32_t count) {
    uint32_t start_index = (start - memory_start) / PAGE_SIZE;
    
    for(uint32_t i = 0; i < count; i++) {
        bitmap_set(memory_bitmap, start_index + i, 1);
        free_pages--;
    }
}