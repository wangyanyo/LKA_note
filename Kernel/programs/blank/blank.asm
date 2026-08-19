[BITS 32]

section .asm

global _start

_start:
label:
	push message
	mov eax, 1
	int 0x80
	add esp, 4
	jmp $

section .data
message: db 'I can talk with the kernel!', 0