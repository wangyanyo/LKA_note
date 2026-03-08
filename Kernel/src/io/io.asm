section .asm

global insb
global insw
global outb
global outw
insb:
        push ebp
        mov ebp, esp

        xor eax, eax
        mov dx, [ebp+8]
        in al, dx

        pop ebp
        ret

insw:
        push ebp
        mov ebp, esp
        
        xor eax, eax
        mov dx, [ebp+8]
        in ax, dx

        pop ebp
        ret

outb:
        push ebp
        mov ebp, esp

        mov al, [ebp+12]
        mov dx, [ebp+8]
        out dx, al

        pop ebp
        ret

outw:
        push ebp
        mov ebp, esp

        mov ax, [ebp+12]
        mov dx, [ebp+8]
        out dx, ax

        pop ebp
        ret