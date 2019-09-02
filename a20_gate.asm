;; File: a20_gate.asm
;; Included by boot_stage1.asm


;; Fuctions regarding the A20 Gate


[bits 16]

A20_gate:
	push si

	call check_A20
	test ax, ax
	jnz A20_enabled

	mov si, A20_disabled_msg
	call bios_print
	jmp bios_int15_24

A20_enabled:
	mov si, A20_enabled_msg
	call bios_print
	jmp A20_done

;; try to enable A20 gate with BIOS call
bios_int15_24:
	push ax

	mov ax, 0x2401		; ah = 24, al = enable/disable
	int 0x15

	call check_A20
	test ax, ax
	jnz A20_ready

	mov si, bios_int15_24_failed
	call bios_print

	cli
	hlt			; BIOS call didn't enable A20

	;
	; TODO: add additional A20 enabling methods
	;
	
A20_ready:
	mov si, bios_int15_24_msg
	call bios_print

	pop ax

A20_done:
	pop si
	ret
		
;; Check if A20 gate is enabled -> return 1 if enabled, else return 0
check_A20:
	; 0x007dfe (0xaa55 magic number)
	; 0x100000 (1 MiB)
	; 0xffff -> 0xffff0 + 0x10 = 0x100000
	; 0xffff -> 0xffff0 + 0x7e0e = 0x107e0e

	push ax
	push bx
	push es
	push ds

	mov ax, 0xffff
	mov es, ax
	xor ax, ax
	mov ds, ax

	mov ax, [es:0x7e0e]
	mov bx, [ds:0x7dfe]
	cmp ax, bx
	mov ax, 1
	jne A20_checked

	rol bx, 3
	mov [ds:0x7dfe], bx
	mov ax, [es:0x7e0e]
	cmp ax, bx	
	mov ax, 1
	jne A20_checked
	xor ax, ax

A20_checked:
	pop ds
	pop es
	pop bx
	pop ax

	ret


A20_enabled_msg:
	db "BIOS-A20: Enabled by default", 0x0a, 0x0d, 0x00
A20_disabled_msg:
	db "BIOS-A20: Disabled by default", 0x0a, 0x0d, 0x00
bios_int15_24_msg:
	db "BIOS-int15/24: A20 gate enabled", 0x0a, 0x0d, 0x00
bios_int15_24_failed:
	db "BIOS-int15/24: Not supported", 0x0a, 0x0d, 0x00
