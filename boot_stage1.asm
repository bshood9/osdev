;; File: boot_stage1.asm
;; Included by boot_stage0.asm


;; Stage 2 Bootloader (yeah the files aren't numbered correctly...)

[bits 16]

%include "bios_print.asm"
%include "a20_gate.asm"
%include "bios_e820.asm"
%include "bios_int13.asm"

%define kernel_size  20 ; size of kernel in sectors (multiple of 512 bytes)
%define kernel_sector 6	; sector where kernel is


load_kernel: ; es:bx -> (es = 0xffff) -> 0xffff0 + (bx = 0x10) == 0x100000
	pusha

	mov dl, [boot_drive]

	test dl, 0x80
	jz .read_from_fdd

	test dl, 0x60
	jz .read_from_hdd

.read_from_cdrom:
	jmp kernel_returned	; cdrom not implemented yet
	jmp .load_kernel_done
	
.read_from_fdd:
	mov ax, 0xffff
	mov es, ax
	mov bx, 0x10		; es:bx -> physical memory address
	mov si, kernel_size
	mov di, kernel_sector

	call fdd_read_sectors

	xor ax, ax
	mov es, ax		; reset es

	jc  .load_kernel_error
	jo  .load_kernel_error
	jmp .load_kernel_done	; fdd read successful

.read_from_hdd:
	mov ax, 0xffff		; buffer segment
	mov bx, 0x10		; buffer offset
	mov cx, kernel_size	; sectors to read
	mov di, kernel_sector	; start sector
	dec di			; 0-indexed
	
	call hdd_read_sectors
	jnc .load_kernel_done	; hdd read successful

.load_kernel_error:
	push ax

	mov si, load_kernel_error_msg
	call bios_print

	pop ax
	mov cl, 1
	call print_hex
	mov si, newline
	call bios_print

	jmp kernel_returned	; failed to load kernel, jump here for now

.load_kernel_done:
	popa
	ret


;; Switch into 32-bit Protected Mode
enable_protected_mode:
	mov cx, 0x18		; words to read/print
	mov dx, 0xffff		; ds = 0xffff
	mov si, 0x19f0
	call writemem		; lods ax, word ptr ds:[si]

	movzx ax, byte [boot_drive]
	call print_hex
;	jmp kernel_returned

	cli			; turn off interrupts

	lgdt [gdtr]		; load global descriptor table

	mov eax, cr0
	or eax, 1		; enable 32-bit protected mode bit
	mov cr0, eax

	jmp 0x08: dword set_sgmt_regs	; cs = kernel code descriptor


[bits 32]
	
;; enter 32-bit protected mode
set_sgmt_regs:
	mov ax, 0x10		; kernel_data selector

	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

protected_mode_enabled:
	mov  eax, 0x2badb002	; multiboot magic number
;	mov  ebx, multiboot_struct
;	push multiboot_struct

	call 0x100000		; hand execution to kernel

kernel_returned:
	cli
.nmi:	hlt
	jmp short .nmi		; NMI or SMM can move EIP after hlt


gdt:
null_descriptor:
	dq 0
kernel_code:
	dw 0xffff		; limit [0:15]
	dw 0x0000		; base [0:15]
	db 0x00			; base [16:23]
	db 10011010b		; access: pr, privl, 1, ex, dc, rw, ac
	db 11001111b		; limit [16:19], flags: gr, sz, 0, 0
	db 0x00			; base [24:31]

kernel_data:
	dw 0xffff
	dw 0x0000
	db 0x00
	db 10010010b
	db 11001111b
	db 0x00

user_code:
	dw 0xffff
	dw 0x0000
	db 0x00
	db 11111010b
	db 11001111b
	db 0x00

user_data:
	dw 0xffff
	dw 0x0000
	db 0x00
	db 11110010b
	db 11001111b
	db 0x00

gdtr:
	dw $-gdt-1
	dd gdt


protected_mode_msg: db "boot: 32-bit Protected mode enabled", 0x0a, 0x0d, 0x00
load_kernel_error_msg: db "Error loading kernel: ", 0x00
kernel_loaded_msg: db "Kernel loaded into high-memory (ie. 0x100000)", 0x0a, 0x0d, 0x00
