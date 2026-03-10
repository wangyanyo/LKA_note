#include "kernel.h"
#include "idt/idt.h"
#include "terminal/print.h"
#include "io/io.h"
#include "memory/heap/kheap.h"

void kernel_main()
{
	terminal_initialize();
	terminal_print("Hello World!\n");
	idt_init();
	kheap_init();

	void *p1 = kmalloc(100);
	terminal_print_hex((unsigned long)p1);
	void *p2 = kmalloc(100);
	terminal_print_hex((unsigned long)p2);
	void *p3 = kmalloc(100);
	terminal_print_hex((unsigned long)p3);
	kfree(p2);
	void *p4 = kmalloc(100);
	terminal_print_hex((unsigned long)p4);
	kfree(p1);
	kfree(p3);
	kfree(p4);
}