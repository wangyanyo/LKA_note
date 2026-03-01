#include "kernel.h"
#include "idt/idt.h"
#include "terminal/print.h"

void kernel_main()
{
	terminal_initialize();
	idt_init();
}