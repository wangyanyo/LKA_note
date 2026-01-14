ORG 0x7C00
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

_start:
        jmp short start
        nop

times 33 db 0

start:
        jmp 0x00:step2

step2:
        cli
        mov ax, 0x00
        mov ds, ax
        mov es, ax
        mov ax, 0x00
        mov ss, ax
        mov sp, 0x7C00
        sti
        
.load_protected:
        cli
        lgdt[gdt_desc]
        mov eax, cr0
        or eax, 0x01
        mov cr0, eax
        jmp CODE_SEG:load32

gdt_start:
gdt_null:
        dd 0
        dd 0

gdt_code:
        dw 0xffff
        dw 0
        db 0
        db 10011010b
        db 11001111b
        db 0

gdt_data:
        dw 0xffff
        dw 0
        db 0
        db 10010010b
        db 11001111b
        db 0
gdt_end:

gdt_desc:
        dw gdt_end - gdt_start - 1
        dd gdt_start

[BITS 32]
load32:
        mov ax, DATA_SEG
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        mov ebp, 0x200000
        mov esp, ebp

        ; Enable A20 line
        in al, 0x92
        or al, 2
        out 0x92, al

        jmp $

times 510 - ($-$$) db 0
dw 0xAA55