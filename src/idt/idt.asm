section .asm
extern int21h_handler
extern no_interrupt_handler
extern isr80h_handler
extern interrupt_handler

global idt_load
global isr80h_wrapper
global enable_interrupts
global disable_interrupts
global interrupt_pointer_table

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

%macro interrupt 1
    global int%1
    int%1:
        ; INTERRUPT FRAME START
        ; ALREADY PUSHED TO US BY THE PROCESSOR UPON ENTRY TO THIS INTERRUPT
        ; uint32_t ip
        ; uint32_t cs;
        ; uint32_t flags
        ; uint32_t sp;
        ; uint32_t ss;
        ; Pushes the general purpose registers to the stack
        pushad
        ; Interrupt frame end
        push esp
        push dword %1
        call interrupt_handler
        add esp, 8
        popad
        iret
%endmacro

%assign i 0
%rep 512
    interrupt i
%assign i i+1
%endrep

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

%macro interrupt_array_entry 1
    dd int%1
%endmacro

interrupt_pointer_table:
%assign i 0
%rep 512
    interrupt_array_entry i
%assign i i+1
%endrep