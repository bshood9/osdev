CC = gcc

CFLAGS = -m32 -ffreestanding -ffunction-sections -fno-asynchronous-unwind-tables -c

SOURCES = kernel.c idt.c exceptions.c vga_text_mode.c lib/string.c pic.c pit.c lib/list.c asm.c drivers/ps2.c drivers/keyboard.c systime.c

OBJECTS = kernel.o idt.o exceptions.o vga_text_mode.o string.o pic.o pit.o list.o asm.o ps2.o keyboard.o systime.o

BINARIES = boot.bin boot1.bin kernel.bin


all:

# Compile kernel
		$(CC) $(CFLAGS) $(SOURCES)


# Link kernel
#ld -m elf_i386 -o kernel.bin -Ttext 0x00100000 --oformat binary kernel.o
		ld -T ld_script $(OBJECTS)


# Pad kernel binary to a multiple of 512 bytes (sector)
		truncate -s %512 kernel.bin


# Change hard-coded "kernel_size" macro in boot_stage1.asm
		./get_kernel_size


# Assemble bootsector/bootloader
		nasm -fbin -o boot.bin mbr.asm
		nasm -fbin -o boot1.bin boot_stage0.asm
		cat boot1.bin >> boot.bin


# Pad bootloader size to a multiple of 512 bytes (sector)
		truncate -s %512 boot.bin


# Concatenate kernel binary to the end of the boot image
		cat kernel.bin >> boot.bin


clean:
#		cp -r . /mnt/4/Documents/osdev
		rm $(OBJECTS) $(BINARIES)


backup:
		cp -r . /mnt/4/Documents/osdev


usb:
		sudo dd if=./boot.bin of=/dev/disk/by-id/usb-SanDisk_Cruzer_174012143781C420-0:0 skip=0 seek=0 conv=fdatasync

run:
		bochs -rc ./bochs_debug_script
