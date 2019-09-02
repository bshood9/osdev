;; File: bios_int13.asm


;; Read sectors from a floppy drive
fdd_read_sectors:	  ; args: si = # of sectors, di = first sector
	pusha

.loop:
	mov  ax, di		; args: es:bx = physical memory address
	call cylinder_head_sector

	mov al, 19
	sub al, cl
	xor ah, ah
	cmp ax, si		; if (sectors we can read >= sectors to read)
	jge .last_read

	sub si, ax		; remaining sectors -= sectors we can read

	mov ah, 0x02		; read sectors routine
	mov dl, [boot_drive]	; drive number
	int 0x13		; disk services
	jc  .fdd_read_error

	xor ah, ah

	add di, ax
	mov dx, 512
	mul dx		  ; sectors read * 512 bytes
	add bx, ax	  ; increment physical memory address to write
	jo .fdd_read_error	; kernel > 64 KiB - 16
	
	jmp .loop

.last_read:
	mov ax, si
	mov ah, 0x02
	mov dl, [boot_drive]
	int 0x13

	xor ax, ax
	mov es, ax		; reset es register

	jnc .fdd_read_done	; read successful

.fdd_read_error:
	mov al, ah
	xor ah, ah

.fdd_read_done:
	popa
	ret


;; Calculate CHS (cylinder/head/sector) given absolute sector
cylinder_head_sector:		; args: ax = absolute sector
	xor dx, dx
	mov cx, 36
	div cx			; dx = remainder, ax = quotient
	; dh = 0, dl = 2 * sector, ah = 0, al = cylinder (+ 1 if ax % 36 == 0)
	test dl, dl		; if (ax % 36 != 0)
	jnz .found_cylinder

	dec al			; al = cylinder

.found_cylinder:
	mov ch, al		; cylinder

	cmp dl, 18
	jg .head1
.head0:
	xor dh, dh		; head = 0
	mov cl, dl		; sector
	jmp .cylinder_head_sector_done
.head1:
	mov dh, 1		; head = 1
	sub dl, 18
	mov cl, dl		; sector

.cylinder_head_sector_done:
	ret


hdd_read_sectors:
	push si

	mov word [disk_address_packet+6], ax ; buffer segment
	mov word [disk_address_packet+4], bx ; buffer offset
	mov word [disk_address_packet+2], cx ; sectors to read
	mov word [disk_address_packet+8], di ; start sector

	mov ah, 0x42		; extended read sectors from drive
	mov si, disk_address_packet
	int 0x13

	pop si
	ret


disk_address_packet:
	db 0x10			; size of DAP
	db 0x00			; unused
	dw 0x0000		; number of sectors to be read
	dw 0x0000		; buffer offset
	dw 0x0000		; buffer segment
	dq 0x0000000000000000	; absolute number of start sector (0-indexed)
