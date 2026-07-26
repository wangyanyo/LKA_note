[BITS 32]
section .asm

global restore_general_purpose_registers
global task_return
global user_registers

; void task_return(struct registers* regs);
task_return:
	mov ebp, esp
	mov ebx, [ebp+4]        ; ebx = struct registers* regs

	; 构建 iretd 栈帧顺序: SS, ESP, EFLAGS, CS, EIP
	push dword [ebx+44]     ; SS
	push dword [ebx+40]     ; ESP

	pushf
	pop eax
	or eax, 0x200           ; 置 IF 位，开启中断
	push eax                ; EFLAGS

	push dword [ebx+32]     ; CS
	push dword [ebx+28]     ; EIP

	; 加载用户态数据段选择子
	mov ax, [ebx+44]
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	push ebx
	call restore_general_purpose_registers
	add esp, 4

	iretd
    
; void restore_general_purpose_registers(struct registers* regs);
restore_general_purpose_registers:
	push ebp
	mov ebp, esp
	mov ebx, [ebp+8]        ; ebx = regs指针

	mov edi, [ebx]
	mov esi, [ebx+4]
	mov ebp, [ebx+8]
	mov ebx, [ebx+12]
	mov edx, [ebx+16]
	mov ecx, [ebx+20]
	mov eax, [ebx+24]

	pop ebp
	ret

; void user_registers();
user_registers:
	mov ax, 0x23
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	ret
