#include "kernel.h"
#include "idt/idt.h"
#include "terminal/print.h"
#include "io/io.h"

void kernel_main()
{
	terminal_initialize();
	terminal_print("Hello World!\n");
	idt_init();
}