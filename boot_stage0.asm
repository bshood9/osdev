[org 0x7e00]			; MBR starts at 0x7c00
[bits 16]


;; Clear segment registers
	xor ax, ax

	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

;; Set stack
	mov bp, 0xffff ; this is a safe location for the boot loader to use
	mov sp, bp

	mov [boot_drive], dl


;; Clear screen
bios_int10_00:
	mov ah, 0x00		; set video mode
	mov al, 0x03		; 80x25 16 color text mode
	int 0x10

	
;; Read the rest of the VBR into memory
;;
;; Boot volume contents (for VBR of size 1536 bytes (3 sectors)):
;;          +-------+-----------------------+--------+
;;          |  MBR  |          VBR          | kernel |
;;          +-------+-----------------------+--------+
;; CHS:     0,0,1   0,0,2                   0,0,5
;; address: 0x000   0x200                   0x800

bios_int13_02:

;; Reset floppy drive (make sure head points to sector 0)
	mov ah, 0x00
	mov dl, [boot_drive]
	int 0x13		; BIOS disk services

;; Read sectors for stage 1 bootloader
	mov ah, 0x02
	mov al, 3		; sectors to read
	mov bx, 0x8000		; es:bx -> base memory address to write
	mov ch, 0		; cylinder
	mov cl, 3		; sector (1-indexed)
	mov dh, 0		; head
	mov dl, [boot_drive]	; drive number
	int 0x13

	jnc int13_02_done	; carry flag set on error

;; Print error code
	mov  [int13_02_error_code], ah
	mov  si, int13_02_error
	call print_err
	mov  si, int13_02_error_code
	call print_err

	jmp boot_failure
	
print_err:			; we can't use "bios_print" from sectors read
	push ax

.loop:
	lodsb
	test al, al
	jz  .print_err_done
	mov  ah, 0x0e
	int  0x10
	jmp .loop

.print_err_done:
	pop ax
	ret


int13_02_done:
	;	mov si, stage2_msg
	;	call bios_print


;; enable the A20 Gate
	call A20_gate		; TODO: figure out why BIOS auto-activates now

	
;; BIOS: query conventional memory size (up to 640 KiB)
bios_int12:
	clc			; clear carry flag (set on failure)
	int 0x12
	jc .int12_failure

	mov  si, int12_msg
	call bios_print
	mov  cl, 1		; select "0x00" hexidecimal notation
	call print_hex
	mov  si, newline
	call bios_print
	jmp .int12_done
	
.int12_failure:
	mov  si, int12_failed_msg
	call bios_print

.int12_done:


;; BIOS-e820 (store descriptor table at 0x500)
	call bios_e820
	jc boot_failure

	call print_e820_descriptors ; print e820 table


stage2:
	call load_kernel	; copy the kernel (in the boot image) to RAM

	;	mov si, kernel_loaded_msg
	;	call bios_print

	jmp enable_protected_mode


boot_failure:
	cli
.loop:	hlt
	jmp short .loop


int12_msg:
	db "BIOS-int12: conventional memory = ", 0x00
int12_failed_msg:
	db "BIOS-int12: returned error", 0x0a, 0x0d, 0x00
int13_02_error:
	db "BIOS-int13/02: Error reading sector, code:  ", 0x00

int13_02_error_code:
	db 0x00, 0x0a, 0x0d, 0x00

stage2_msg:
	db "boot: Loaded stage 2 bootloader", 0x0a, 0x0d, 0x00

newline:
	db 0x0a, 0x0d, 0x00

boot_drive:
	db 0x00			; save value of dl


	times 510-($-$$) db 0x00
	dw 0xaa55


;;%include "boot_stage1.asm"
%include ".stage1_correct_kernel_size.asm"
