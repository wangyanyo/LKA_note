#include "kernel.h"
#include "idt/idt.h"
#include "terminal/print.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"

struct paging_4gb_trunk *kernel_trunk = 0;

void kernel_main()
{
	terminal_initialize();
	terminal_print("Hello World!\n");

	kheap_init();

	idt_init();

	kernel_trunk = paging_new_4gb_trunk(PAGING_IS_WRITEABLE | PAGING_IS_PERSENT | PAGING_ACCESS_FROM_ALL);
	paging_switch(paging_4gb_trunk_get_directory(kernel_trunk));
	enable_paging();

	enable_interrupts();
}