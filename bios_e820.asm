;; File: bios_e820.asm


;; BIOS-e820: Query system address map


[bits 16]
%define e820_mmap 0x500 ; location to store e820 mmap


bios_e820:
	mov eax, 0xe820		; routine number
	xor ebx, ebx		; continue?
	mov ecx, 24		; structure size (20 bytes minimum)
	mov edx, 0x534d4150	; magic number (SMAP signature)
	mov di, e820_mmap	; es:di -> destination buffer
	clc			; clear carry flag
	int 0x15

	jc  e820_failed		; CF clear on success
	cmp eax, 0x534d4150	; should be equal on success
	jne e820_failed
	cmp ecx, 20		; if (ecx < 20 bytes): error
	jl  e820_failed
	cmp ecx, 24		; if (ecx > 24 bytes): error
	jg  e820_failed

get_next_descriptor:
	mov [di+20], dword 1  ; ACPI attribute (entry is non-volatile)
	test ebx, ebx
	jz   e820_done

	mov eax, 0xe820
	mov ecx, 24
	add di, 24
	clc
	int 0x15

	jc  e820_done
	cmp eax, 0x534d4150
	jne e820_failed

	jmp get_next_descriptor

e820_failed:
	mov  si, e820_failed_msg
	call bios_print
	
	stc
	ret			; return failure

e820_done:

	clc
	ret			; return success


print_e820_descriptors:
	push di

	mov  di, e820_mmap
	mov  si, bios_e820_table_msg
	call bios_print

print_descriptor:
	mov  si, bios_e820_msg
	call bios_print

	mov cl, 1		; print "0x" suffix

print_addr:
	call print_qw_hex

	inc  si
	call bios_print

	add  di, 8
	call print_qw_hex

	inc  si
	call bios_print
	
	add di, 8

	mov  ebx, [di]
	test ebx, 0xfffffff0	; we only care about the low 3 bits
	jnz  e820_type_undefined
	test bx, bx
	jz   e820_type_undefined	; assert bx > 0
	cmp  bx, 5
	jg   e820_type_undefined	; assert bx <= 5
	dec  bx
	shl  bx, 1
	add  bx, e820_switch
	jmp [bx]		; switch (bx)

e820_type_free:	
	mov  si, e820_usable_msg
	call bios_print

	jmp next_descriptor
	
e820_type_reserved:
	mov  si, e820_reserved_msg
	call bios_print

	jmp next_descriptor

e820_type_acpi:
	mov  si, e820_acpi_data_msg
	call bios_print

	jmp next_descriptor

e820_type_acpi_nvs:
	mov  si, e820_acpi_nvs_msg
	call bios_print

	jmp next_descriptor

e820_type_bad:
	mov  si, e820_bad_msg
	call bios_print

	jmp next_descriptor

e820_type_undefined:
	mov  si, e820_undefined_msg
	call bios_print

next_descriptor:
	sub di, 16
	pop dx
	cmp di, dx
	je  e820_print_done

	push dx	
	add  di, 24
	jmp  print_descriptor

e820_print_done:
;	mov si, newline
;	call bios_print

	ret

	
;e820_descriptor_struct:
;	dd 0x00000000		; base address low 32 bits
;	dd 0x00000000		; base address high 32 bits
;	dd 0x00000000		; length low 32 bits
;	dd 0x00000000		; length high 32 bits
;	dd 0x00000000		; 1:free,2:rsvd,3:ACPI reclm,4:ACPI NVS,5:bad
;	dd 0x00000000		; ACPI attributes


e820_switch:			; switch statement jmp-array
	dw e820_type_free
	dw e820_type_reserved
	dw e820_type_acpi
	dw e820_type_acpi_nvs
	dw e820_type_bad


e820_type_names:
	e820_usable_msg         db "usable", 0x0a, 0x0d, 0x00
	e820_reserved_msg	db "reserved", 0x0a, 0x0d, 0x00
	e820_acpi_data_msg	db "ACPI data", 0x0a, 0x0d, 0x00
	e820_acpi_nvs_msg	db "ACPI NVS", 0x0a, 0x0d, 0x00
	e820_bad_msg            db "bad", 0x0a, 0x0d, 0x00
	e820_undefined_msg      db "undefined", 0x0a, 0x0d, 0x00


bios_e820_table_msg:
	db "e820: BIOS-provided physical RAM map:", 0x0a, 0x0d, 0x00
bios_e820_msg:
	db "BIOS-e820: base: ", 0x00, "  limit: ", 0x00, "  type: ", 0x00
e820_failed_msg:
	db "BIOS-e820: address map query failed", 0x0a, 0x0d, 0x00
