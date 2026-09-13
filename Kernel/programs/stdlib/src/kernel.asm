[BITS 32]
global print:function
global getkey:function
global putchar:function
global kernel_malloc:function

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
getkey:
	push ebp
	mov ebp, esp
	mov eax, 2
	int 0x80
	pop ebp
	ret

putchar:
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