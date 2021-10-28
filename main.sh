#install-pkg qemu # RUN ON STARTUP

# clean up terminal
clear
echo

# assembles bootloader
nasm -f elf32 src/boot.asm -o boot.o

# compiles kernel
gcc src/kernel.c -c -m32 -fno-pic -ffreestanding -Wno-int-conversion -fno-asynchronous-unwind-tables -o kernel.o

ld boot.o kernel.o --oformat binary -m elf_i386 -Ttext 0x7C00 -o os.bin # links kernel

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.  qemu-img create myOS.img 16384 # creates virtual disk image with 16 KB of storage
dd if=os.bin of=myOS.img bs=2048 count=1 # copies OS  to disk image

mv myOS.img cache # caches disk image
mv os.bin cache # caches os

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.  qemu-system-i386 -drive format=raw,file=cache/myOS.img
