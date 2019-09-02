[org 0x7c00]
[bits 16]

	jmp chain_load	; also acceptable: jmp near
	nop
	
OEM_name:
	db "Unnamed0"


;	times 79 db 0
BIOS_parameter_block:
;; DOS 2.0 BPB
	dw 0  ; bytes per logical sector
	db 0  ; logical sectors per cluster
	dw 0  ; reserved logical sectors
	db 0  ; number of FATs
	dw 0  ; root directory entries
	dw 0  ; total logical sectors
	db 0  ; media descriptor
	dw 0  ; logical sectors per FAT

;; DOS 3.31 BPB
	dw 0  ; physical sectors per track
	dw 0  ; number of heads
	dd 0  ; hidden sectors
	dd 0  ; large logical disk sectors (partition table not liked)


;; Chainload the VBR
chain_load:
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov ss, ax

	mov bp, 0xffff
	mov sp, bp

	push dx

	mov ah, 0x00		; reset disk
	int 0x13

	jc critical_error

	cmp dl, 0xe0
	jz cdrom_read_vbr
	test dl, 0x80
	jnz hdd_read_vbr


fdd_read_vbr:
	mov ah, 0x02
	mov al, 1		; sectors to read
	mov bx, 0x7e00		; es:bx -> buffer
	mov ch, 0		; cylinder
	mov cl, 2		; sector
	pop dx
	mov dh, 0		; head
	int 0x13

	jnc 0x7e00		; Volume Boot Record
	jmp critical_error


hdd_read_vbr:
	pop dx
	mov ah, 0x42		; extended read sectors from drive
	mov si, struct_DAP	; Disk Address Packet
	int 0x13

	jnc 0x7e00		; Volume Boot Record
	jmp critical_error


;; not implemented
cdrom_read_vbr:
	mov  si, cd_msg1
	call print_error
	mov  si, cd_msg2
	call print_error

	jmp critical_error

print_error:
.loop:
	lodsb
	test al, al
	jz  .print_done
	mov  ah, 0x0e
	int  0x10
	jmp .loop

.print_done:
	ret

	cd_msg1	db "Storage type: CD-ROM", 0x0a, 0x0d, 0x00
	cd_msg2	db "Error: cannot read from CD-ROM", 0x0a, 0x0d, 0x00


;; Critical error encountered: hang CPU
critical_error:
	cli
.loop:	hlt
	jmp short .loop


;; Disk Address Packet Structure
struct_DAP:
	db 0x10			; size of DAP
	db 0			; unused
	dw 1			; number of sectors to be read
	dw 0x7e00		; buffer offset
	dw 0			; buffer segment
	dq 1			; absolute number of start sector (0-indexed)
.end_struct:


;; Pad MBR size to 512 bytes
	times 504-($-$$)-(struct_ptable.end_struct - struct_ptable) db 0


;; Special Reserved bytes
	dd 0						       ; reserved
	dw 0						       ; free


;; Partition Table Structure
struct_ptable:
	db 0x80	 ; bit 7 set/clear -> active/invalid, 0x00 -> inactive
.first_CHS_address:
	db 0			; bits: head [7:0]
	db 2			; bits: cylinder [9:8], sector [0:5]
	db 0			; bits: cylinder [7:0]
.partition_type:
	db 0x0c			; FAT32 with LBA
.last_CHS_address:		; same format as above^
	db 0
	db 3
	db 0
.LBA_address:
	dd 1			; first sector in partition
	dd 1			; number of sectors

	times 48 db 0		; partitions 2-4 are empty
.end_struct:


;; Boot Signature
	dw 0xaa55
