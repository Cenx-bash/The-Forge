# Zenin OS Makefile - Advanced build system
# Tokyo Night Theme - Expert Architecture

# Tokyo Night Color Palette Constants
TOKYO_NIGHT_BG     = 0x1a1b26
TOKYO_NIGHT_FG     = 0xc0caf5
TOKYO_NIGHT_BLUE   = 0x7aa2f7
TOKYO_NIGHT_PURPLE = 0x9d7cd8
TOKYO_NIGHT_CYAN   = 0x7dcfff
TOKYO_NIGHT_GREEN  = 0x9ece6a
TOKYO_NIGHT_YELLOW = 0xe0af68
TOKYO_NIGHT_RED    = 0xf7768e

# Compiler and flags
CC = gcc
AS = nasm
LD = ld
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
         -nostartfiles -nodefaultlibs -Wall -Wextra -Werror \
         -I./src/kernel/include -O2 -std=gnu11
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

# Colors for output
COLOR_RESET = \033[0m
COLOR_BLUE = \033[94m
COLOR_CYAN = \033[96m
COLOR_PURPLE = \033[95m
COLOR_GREEN = \033[92m

# Output directory
BUILD_DIR = build
ISO_DIR = iso
BIN_NAME = zenin_kernel.bin
ISO_NAME = zenin-os.iso

# Source files
KERNEL_SOURCES = $(wildcard src/kernel/kernel/*.c)
LIB_SOURCES = $(wildcard src/kernel/lib/*.c)
ASM_SOURCES = src/boot/boot.asm src/boot/multiboot.asm

# Object files
OBJECTS = $(patsubst src/boot/%.asm,$(BUILD_DIR)/boot/%.o,$(ASM_SOURCES)) \
          $(patsubst src/kernel/%.c,$(BUILD_DIR)/kernel/%.o,$(KERNEL_SOURCES)) \
          $(patsubst src/kernel/%.c,$(BUILD_DIR)/lib/%.o,$(LIB_SOURCES))

# Phony targets
.PHONY: all clean run debug iso

# Default target
all: $(BUILD_DIR)/$(BIN_NAME)

# Create build directories
$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	@echo "$(COLOR_CYAN)Compiling $<...$(COLOR_RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: src/%.asm
	@mkdir -p $(dir $@)
	@echo "$(COLOR_PURPLE)Assembling $<...$(COLOR_RESET)"
	@$(AS) $(ASFLAGS) $< -o $@

# Link kernel
$(BUILD_DIR)/$(BIN_NAME): $(OBJECTS)
	@echo "$(COLOR_BLUE)Linking kernel...$(COLOR_RESET)"
	@$(LD) $(LDFLAGS) -o $@ $^
	@echo "$(COLOR_GREEN)Kernel built: $@$(COLOR_RESET)"

# Create ISO
iso: $(BUILD_DIR)/$(BIN_NAME)
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(BUILD_DIR)/$(BIN_NAME) $(ISO_DIR)/boot/
	@echo "Creating GRUB config with Tokyo Night theme..."
	@echo 'set timeout=10' > $(ISO_DIR)/boot/grub/grub.cfg
	@echo 'set default=0' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo 'menuentry "Zenin OS - Tokyo Night" {' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo '  multiboot /boot/$(BIN_NAME)' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo '  boot' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo '}' >> $(ISO_DIR)/boot/grub/grub.cfg
	@grub-mkrescue -o $(ISO_NAME) $(ISO_DIR)
	@echo "$(COLOR_GREEN)ISO created: $(ISO_NAME)$(COLOR_RESET)"

# Run in QEMU
run: $(BUILD_DIR)/$(BIN_NAME)
	@echo "$(COLOR_PURPLE)Starting Zenin OS in QEMU...$(COLOR_RESET)"
	@qemu-system-x86_64 -kernel $(BUILD_DIR)/$(BIN_NAME) \
		-m 512M \
		-serial stdio \
		-vga std \
		-no-reboot \
		-no-shutdown

# Debug with GDB
debug: $(BUILD_DIR)/$(BIN_NAME)
	@echo "$(COLOR_CYAN)Starting Zenin OS in debug mode...$(COLOR_RESET)"
	@qemu-system-x86_64 -kernel $(BUILD_DIR)/$(BIN_NAME) \
		-m 512M \
		-s -S \
		-serial stdio \
		-vga std \
		-no-reboot \
		-no-shutdown

# Clean build
clean:
	@rm -rf $(BUILD_DIR) $(ISO_DIR) $(ISO_NAME)
	@echo "$(COLOR_GREEN)Clean complete.$(COLOR_RESET)"

# Help
help:
	@echo "$(COLOR_BLUE)Zenin OS Build System$(COLOR_RESET)"
	@echo "$(COLOR_CYAN)make all$(COLOR_RESET)    - Build the kernel"
	@echo "$(COLOR_CYAN)make iso$(COLOR_RESET)    - Create bootable ISO"
	@echo "$(COLOR_CYAN)make run$(COLOR_RESET)    - Run in QEMU"
	@echo "$(COLOR_CYAN)make debug$(COLOR_RESET)  - Run with GDB support"
	@echo "$(COLOR_CYAN)make clean$(COLOR_RESET)  - Clean build files"
