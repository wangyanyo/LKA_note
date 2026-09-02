[BITS 32]

section .asm

global _start

_start:
label:
	call getkey
	push message
	mov eax, 1
	int 0x80
	add esp, 4
	jmp $

; 轮询获取键盘输入，测试效果是敲击键盘前，终端无打印；敲击键盘后，终端打印'I can talk with the kernel!'
getkey:
	mov eax, 2
	int 0x80
	cmp eax, 0x00
	nop
	je getkey
	ret

section .data
message: db 'I can talk with the kernel!', 0