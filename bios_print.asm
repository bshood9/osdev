;; File: bios_print.asm
;; Included by boot_stage1.asm


;; BIOS string utilities


[bits 16]
	
bios_print:
	push ax

.loop:
	lodsb
	test al, al
	jz  .bios_print_done
	mov  ah, 0x0e
	int  0x10
	jmp .loop

.bios_print_done:
	pop ax
	ret


;; prints hex value in ax register
print_hex:			; ax -> number, cl -> 1 = prefix, 2 = suffix
	push bx
	push dx

	mov bx, ax

	test cl, 1		; print "0x" hexidecimal prefix
	jz  .no_hex_prefix

	mov ah, 0x0e
	mov al, "0"
	int 0x10
	mov al, "x"
	int 0x10

.no_hex_prefix:	
	mov al, bh
	shr al, 4

	cmp al, 10
	jl .ah_high_lt_ten

.ah_high_ge_ten:
	sub  al, 10
	add  al, "A"
	jmp .print_ah_high
.ah_high_lt_ten:
	add al, "0"

.print_ah_high:
	mov ah, 0x0e
	int 0x10

	mov al, bh
	shl al, 4
	shr al, 4

	cmp al, 10
	jl .ah_low_lt_ten

.ah_low_ge_ten:
	sub  al, 10
	add  al, "A"
	jmp .print_ah_low
.ah_low_lt_ten:
	add al, "0"

.print_ah_low:
	int 0x10

	mov al, bl
	shr al, 4

	cmp al, 10
	jl .al_high_lt_ten

.al_high_ge_ten:
	sub  al, 10
	add  al, "A"
	jmp .print_al_high
.al_high_lt_ten:
	add al, "0"

.print_al_high:
	int 0x10

	mov al, bl
	shl al, 4
	shr al, 4

	cmp al, 10
	jl .al_low_lt_ten

.al_low_ge_ten:
	sub  al, 10
	add  al, "A"
	jmp .print_al_low
.al_low_lt_ten:
	add al, "0"

.print_al_low:
	int 0x10

	test cl, 2		; print "h" hex suffix
	jz  .no_hex_suffix

	mov al, "h"
	int 0x10

.no_hex_suffix:
.print_hex_done:
	pop dx
	pop bx

	ret


print_qw_hex:			; print a quadword pointed to by di
	push cx

	test cl, 1
	jz  .no_qw_prefix

	mov ah, 0x0e
	mov al, "0"
	int 0x10
	mov al, "x"
	int 0x10
	and cl, 0xfe		; clear bit 0

.no_qw_prefix:
	mov  ax, [di+6]
	call print_hex
	mov  ax, [di+4]
	call print_hex
	mov  ax, [di+2]
	call print_hex
	mov  ax, [di]
	call print_hex

	test cl, 2
	jz  .no_qw_suffix

	mov ah, 0x0e
	mov al, "h"
	int 0x10

.no_qw_suffix:
.print_qw_hex_done:
	pop cx

	ret

	
;; Dump memory to screen (for debug purposes)
writemem:			; args: cx = number of words, dx:si = ds:si
	push si
	and  cx, 0xfff8		; 16 byte aligned

	mov  si, writemem_msg1
	call bios_print

	mov  ax, cx
	shl  ax, 1
	or   cx, 0x01		; print hex prefix
	call print_hex

	mov  si, writemem_msg2
	call bios_print

	pop ax			; ax = si
	sub sp, 2		; keep si on the stack

	call .calc_addr

	rol  eax, 16		; print most significant word
	call print_hex

	and  cx, 0xfffe		; don't print hex prefix
	rol  eax, 16		; print least significant word
	call print_hex

	mov  si, hex_index	; print byte 'legend'
	call bios_print

	pop si			; reload si
	sub sp, 2		; keep si on the stack

.print_words:
	test cx, 0x0007
	jz  .write_nextline

.write_space:
	mov al, " "		; 0x20
	call putchar

.read:
	mov ds, dx
	lodsw

	push cx
	xor  cx, cx
	mov  ds, cx
	xchg ah, al
	call print_hex
	pop  cx

	loop .print_words

.writemem_done:
	pop si
	ret

.write_nextline:
	push si
	mov  si, writemem_newline ; print newline
	call bios_print
	pop  si

	mov   ax, si
	call .calc_addr

	rol  eax, 16		; print most significant word
	call print_hex
	rol  eax, 16		; print least significant word
	call print_hex

	mov  al, ":"
	call putchar
	mov  al, " "
	call putchar

	jmp .read

.calc_addr:
	movzx eax, ax
	movzx edx, dx
	shl   edx, 4
	add   eax, edx
	shr   edx, 4

	ret


putchar:
	push si

	mov  byte [print_char], al
	mov  si, print_char
	call bios_print

	pop si
	ret

print_char: db 0x00, 0x00
hex_index: db 0x0a, 0x0d, "87654321  0011 2233 4455 6677 8899 AABB CCDD EEFF", 0x00
writemem_msg1: db "writemem: ", 0x00
writemem_msg2: db " bytes @ ", 0x00
writemem_newline: db 0x0a, 0x0d, 0x00
