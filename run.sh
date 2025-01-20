#!/bin/bash
set -xue

# QEMU file path
QEMU=qemu-system-riscv32

# Clang and compiler flags
CC=clang
CFLAGS="-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32 -ffreestanding -nostdlib"

# Build the kernel
#
# -Wl, -Tkernel.ld specifies the kernel script
# -Wl, -map=kernel.map outputs a map file (linker allocation result)
$CC $CFLAGS -Wl,-Tkernel.ld -Wl,-Map=kernel.map -o kernel.elf kernel.c

# Start QEMU
$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot -kernel kernel.elf
