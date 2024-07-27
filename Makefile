# Toolchain
AS = nasm
CC = gcc
LD = ld

# Automatically find sources
KERNEL_S_SOURCES = $(shell cd kernel && find -L * -type f -name '*.S')
KERNEL_C_SOURCES = $(shell cd kernel && find -L * -type f -name '*.c')

# Get object files
KERNEL_OBJS := $(addprefix bin/kernel/, $(KERNEL_S_SOURCES:.S=.S.o) $(KERNEL_C_SOURCES:.c=.c.o))

# Flags
ASFLAGS = -f elf32 -Wall -g -F dwarf
CCFLAGS = -m32 -std=gnu11 -ffreestanding -O0 -Wall -Wextra -nostdlib -I kernel -fno-stack-protector
QEMUFLAGS = -debugcon stdio -m 256M -cdrom bin/upOS.iso -boot d

# Output image name
IMAGE_NAME = upOS

all: dirs boot kernel iso

run: all
	qemu-system-i386 $(QEMUFLAGS)

run-gtk: all
	qemu-system-i386 $(QEMUFLAGS) -display gtk,zoom-to-fit=on

run-sdl:
	qemu-system-i386 $(QEMUFLAGS) -display sdl

run-gdb: all
	qemu-system-i386 $(QEMUFLAGS) -S -s

run-amd:
	qemu-system-i386 $(QEMUFLAGS) -cpu phenom,model_id="Testing AMD processor (phenom)",vendor=AuthenticAMD

run-intel:
	qemu-system-i386 $(QEMUFLAGS) -cpu Snowridge,model_id="Testing Intel processor (Snowridge)",vendor=GenuineIntel

dirs:
	mkdir -p bin

bin/kernel/%.c.o: kernel/%.c
	mkdir -p "$$(dirname $@)"
	$(CC) $(CCFLAGS) -c $< -o $@

bin/kernel/%.S.o: kernel/%.S
	mkdir -p "$$(dirname $@)"
	$(AS) $(ASFLAGS) -o $@ $<

kernel: $(BOOT_OBJS) $(KERNEL_OBJS)
	$(LD) -m elf_i386 -Tkernel/linker.ld $^ -o bin/kernel.elf

iso:
	grub-file --is-x86-multiboot bin/kernel.elf
	mkdir -p iso_root/boot/grub/
	cp bin/kernel.elf iso_root/boot/kernel.elf
	cp boot/grub.cfg iso_root/boot/grub/grub.cfg
	grub-mkrescue -o bin/upOS.iso iso_root/
	rm -rf iso_root/

clean:
	rm -f $(BOOT_OBJS) $(KERNEL_OBJS)
	rm -rf bin