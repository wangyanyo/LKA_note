#include "kernel.h"
#include "idt/idt.h"
#include "terminal/print.h"
#include "io/io.h"

void kernel_main()
{
	terminal_initialize();
	idt_init();
	outb(0x60, 0xff);
}