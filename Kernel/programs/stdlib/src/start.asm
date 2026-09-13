[BITS 32]

global _start
extern main

section .asm

; 在linker.ld中指明了起始的符号是_start，但是原有的C程序没有_start，所以编译出一个stdlib.elf并和blank.o链接在一起
_start:
    call main