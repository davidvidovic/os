#!/bin/bash

if [ -z "$PREFIX" ]; then
    echo "PREFIX is not set or is empty."
else
    echo "PREFIX is set to: $PREFIX"
fi

if [ -z "$TARGET" ]; then
    echo "TARGET is not set or is empty."
else
    echo "TARGET is set to: $TARGET"
fi

echo "Starting build..."

i686-elf-as boot.s -o boot.o
echo "Assembled boot.s"

i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
echo "Compiled kernel.c"

i686-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc
echo "Linked linker.ld"

echo -e "\e[1;32mBuild finished successfully!\e[0m"

qemu-system-i386 -kernel myos.bin

