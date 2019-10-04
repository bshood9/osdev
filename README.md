# osdev

This repository contains a Master Boot Record (MBR) / boot sector, a bootloader, and a limited kernel with some bare-metal driver support -- in other words, the beginnings of a "toy" OS.

The repository's contents are described in more detail below.


## MBR and Bootloader

The BIOS loads the MBR into memory, then the MBR loads the first sector (512 bytes) of the bootloader into memory (also known as the "Volume Boot Record"), then execution is handed to the bootloader, and then the bootloader loads the rest of itself into memory. The bootloader then attempts to enable the A20 Gate (if it's not already enabled), queries the system address map, and does some other stuff, before loading the kernel into high-memory (at address 1 MiB). After loading the kernel, it switches the CPU into 32-bit protected mode and calls the kernel.

For debugging purposes, the bootloader has various routines for printing strings and dumping memory.

The bootloader is not currently multiboot-compatible.


#### mbr.asm
This is the Master Boot Record

#### boot_stage0.asm
This is the first 512 bytes of the bootloader (a.k.a. Volume Boot Record)

#### boot_stage1.asm
This is the rest of the bootloader

#### a20_gate.asm
Routines for handling the A20 Gate

#### bios_e820.asm
Routines for querying the system address map

#### bios_int13.asm
Routines for reading from FDDs and HDDs

#### bios_print.asm
Routines for printing strings and dumping memory


## Kernel "core"

The kernel has limited functionality at this point. One of its features is management of the Interrupt Descriptor table; this allows for Interrupt Service Routines to be installed for devices or software interrupts. The kernel also manages a list of *timed_service* structs and iterates through them every time the PIT Interrupt Service Routine is invoked. These *timed_service* structs can be set to point to routines to be called upon wait expiration.

The kernel has variable *ktime* that gets updated every PIT interrupt (by means of a *timed_service* pointing to a function that performs the update). *ktime* is used to keep track of how much time has passed since the kernel has started running (or more specifically, how much time has passed since the kernel initialized the IDT, PIC, and PIT -- as elapsed time cannot be tracked without initializing these first).

For testing purposes, the kernel has routine *print_text_clock()* that prints a human-readable *ktime* to the sceen every PIT interrupt (again, by means of a *timed_service*).


#### kernel.c
Kernel's entry point; calls various routines to set up the IDT, PIC, PIT, and PS/2 controller; spins in a while-loop once everything is set up

#### exceptions.h
#### exceptions.c
Interrupt Service Routines for hardware exceptions

#### io.h
Wrapper-routines for x86 "in" and "out" instructions

#### idt.h
#### idt.c
Routines for handling the Interrupt Descriptor Table

#### systime.h
#### systime.c
Routines for maintaining a list of *timed_service* structs, an IRQ spin-wait, and the PIT Interrupt Service Routine

#### time.h
Contains the declaration for a time_t struct

#### time.c
Contains routines that are identical to the inline routines in systime.h (and could therefore be deleted)

#### vga_text_mode.h
#### vga_text_mode.c
Routines for drawing in VGA 80x25 16-color text-mode

#### asm.h
Macros for x86 instructions

#### asm.c
Wrapper-routines for x86 instructions; not currently used; these are pretty useless since there exists inline assembly


## Drivers

The PIC is initialized by sending it a sequence of four control words. The PIT is initialized by sending it a single control word, followed by a 2-byte count (to set the counter). Both of these devices have complex features that my code doesn't use (some of which are even *unavailable* to IBM PC-compatible systems). The PS/2 controller is a much more sophisticated microcontroller than the PIC and the PIT. Correspondingly, it has a much more sophisticated initialization process, and many more features than we care about.

The PIC and the PIT files are located in the root directory rather than the "drivers" subdirectory; this is an inconsistency, and I will probably relocate them to the "drivers" subdirectory at some point...


#### pic.h
#### pic.c
Routines for handling the 8259 Programmable Interrupt Controller (PIC)

#### pit.h
#### pit.c
Routines for handling the 8253 Programmable Interval Timer (PIT)

#### drivers/keyboard.h
#### drivers/keyboard.c
MF2 keyboard Interrupt Service Routine

#### drivers/ps2.h
#### drivers/ps2.c
Routines for handling the 8042 PS/2 controller


## Libraries

The list library implements a doubly-linked list with "head" connected to "tail" -- in other words, a circular buffer. systime.c uses it to manage its list of *timed_service* structs. The string library is self-explanatory; it's basically a clone of <string.h>, except my *strstr()* implementation is wrong.


#### lib/list.h
#### lib/list.c
Routines for managing a doubly-linked list / circular buffer

#### lib/string.h
#### lib/string.c
Routines for manipulating strings; clone of <string.h>


## Miscellaneous files and scripts


#### get_kernel_size
sed script that updates the *kernel_size* macro in boot_stage1.asm with the size of the freshly-compiled kernel; this script is only meant to be used by the Makefile

#### ld_script
Script file for the linker; it ensures that the kernel's *main()* precedes the rest of the kernel's code and data (because the bootloader assumes that the kernel's entry-point is at address 1 MiB); ld_script also passes additional options to the linker

#### Makefile
Builds the mbr, bootloader, and kernel, and concatenates them onto each other
