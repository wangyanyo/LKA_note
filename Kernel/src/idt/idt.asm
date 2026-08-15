section .asm
extern int21h_handler
extern no_interrupt_handler
extern isr80h_handler
global int21h
global idt_load
global no_interrupt
global isr80h_wrapper
global enable_interrupts
global disable_interrupts

idt_load:
	push ebp
	mov ebp, esp
	mov ebx, [ebp+8]
	lidt [ebx]
	pop ebp
	ret

enable_interrupts
	sti
	ret

disable_interrupts
	cli
	ret

int21h:
	pushad
	call int21h_handler
	popad
	iret

no_interrupt:
	pushad
	call no_interrupt_handler
	popad
	iret

isr80h_wrapper:
	; 寄存器现场
	pushad
	; esp指向当前寄存器现场，将其push到栈中作为isr80h_handler函数的参数
	push esp
	; eax中存储了系统调用号，将其push到栈中作为isr80h_handler函数的参数
	push eax
	call isr80h_handler
	; 保存返回值
	mov dword[tmp_res], eax
	; 恢复栈指针
	add esp, 8
	; 弹出寄存器现场，恢复刚进入中断的状态
	popad
	; 返回值赋值
	mov eax, [tmp_res]
	; 中断返回
	iretd

section .data
; 临时存储系统调用返回值
tmp_res: dd 0