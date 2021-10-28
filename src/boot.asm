[BITS 16]

boot1:

mov [disk], dl

; set up stack
cli
mov ax, 0x2000
mov ss, ax
mov sp, 0x1000
mov bp, sp
sti

; set VGA text mode
mov ah, 0x00
mov al, 0x03
int 0x10

; enable A20 line
mov ax, 0x2401
int 0x15

; reset disk system for clean read
mov ah, 0x00
mov dl, [disk]
int 0x13

; read second stage and kernel from disk
mov ah, 0x02
mov al, 8
mov bx, 0x0000
mov es, bx
mov bx, 0x7E00
mov ch, 0
mov cl, 2
mov dh, 0
mov dl, [disk]
int 0x13

jmp [es:bx]

cli
hlt

print:
	mov ah, 0x0E
.loop:
	lodsb
	cmp al, 0
	je .end
	int 0x10
	jmp .loop
.end:
	ret

disk: db 0

msg: db "Booting...", 10, 13, 0

times 510-($-$$) db 0
dw 0xAA55

boot2:

mov si, msg2
call print

lgdt [gdt_desc]
mov eax, cr0
or eax, 0x01
mov cr0, eax

jmp CODE_SEGMENT:boot3

boot3:

[BITS 32]
[EXTERN kmain]

mov ax, DATA_SEGMENT
mov es, ax
mov gs, ax
mov ds, ax
mov fs, ax

call kmain

cli
hlt

msg2: db "Reached stage 2!", 10, 13, 0

gdt_start: ; null segment
dq 0x00
gdt_code: ; code_segment
	dw 0xFFFF
	dw 0x00
	db 0x00
	db 10011010b
	db 11001111b
	db 0x00
gdt_data: ; data_segment
	dw 0xFFFF
	dw 0x00
	db 0x00
	db 10011010b
	db 11001111b
	db 0x00
gdt_end:
gdt_desc: ; gdt descriptors
	dw gdt_end-gdt_start
	dd gdt_start
	CODE_SEGMENT equ gdt_code-gdt_start
	DATA_SEGMENT equ gdt_data-gdt_start

times 1024-($-$$) db 0
