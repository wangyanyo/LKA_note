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

	char *ptr = kzalloc(4096);
	paging_set(paging_4gb_trunk_get_directory(kernel_trunk), 0x1000, (unsigned long)ptr | PAGING_IS_WRITEABLE | PAGING_IS_PERSENT | PAGING_ACCESS_FROM_ALL);

	enable_paging();

	char *ptr_2 = (char *)(0x1000);
	ptr_2[0] = 'A';
	ptr_2[1] = 'B';
	ptr_2[2] = 0;
	
	terminal_print(ptr);

	enable_interrupts();
}