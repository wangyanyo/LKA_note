#include "kernel.h"
#include "idt/idt.h"
#include "terminal/print.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"

struct paging_4gb_chunk *kernel_chunk = 0;

void kernel_main()
{
	terminal_initialize();
	terminal_print("Hello World!\n");

	kheap_init();

	disk_search_and_init();

	idt_init();

	kernel_chunk = paging_new_4gb_chunk(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
	paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));

	/* test *********************************************************/
	char* ptr = kzalloc(4096); 
    	paging_set(paging_4gb_chunk_get_directory(kernel_chunk), (void*)0x1000, (uint32_t)ptr | PAGING_ACCESS_FROM_ALL | PAGING_IS_PRESENT | PAGING_IS_WRITEABLE);
	/***************************************************************/

	enable_paging();

	/* test *********************************************************/
	char *ptr2 = (char *)(0x1000);
	ptr2[0] = 'A';
	ptr2[1] = 'B';
	ptr2[2] = '\n';
	ptr2[3] = 0;
	terminal_print(ptr2);
	terminal_print(ptr);
	terminal_print(ptr);
	/***************************************************************/

	enable_interrupts();
}