[BITS 32]
section .asm
global print:function
global kernel_getkey:function
global kernel_putchar:function
global kernel_malloc:function
global kernel_free:function

; void print(const char* filename)
print:
	push ebp
	mov ebp, esp
	push dword[ebp+8]
	mov eax, 1 ; Command print
	int 0x80
	add esp, 4
	pop ebp
	ret

; int getkey()
kernel_getkey:
	push ebp
	mov ebp, esp
	mov eax, 2
	int 0x80
	pop ebp
	ret

kernel_putchar:
	push ebp
	mov ebp, esp
	push dword[ebp+8]
	mov eax, 3
	int 0x80
	add esp, 4
	pop ebp
	ret

; void *malloc(size_t size)
kernel_malloc:
	push ebp
	mov ebp, esp
	push dword[ebp+8]
	mov eax, 4
	int 0x80
	add esp, 4
	pop ebp
	ret

kernel_free:
	push ebp
	mov ebp, esp
	push dword[ebp+8]
	mov eax, 5
	int 0x80
	add esp, 4
	pop ebp
	ret