#include "kernel.h"
#include "idt/idt.h"
#include "terminal/print.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"
#include "fs/pparser.h"
#include "disk/streamer.h"
#include "fs/file.h"
#include "string/string.h"

struct paging_4gb_chunk *kernel_chunk = 0;

void kernel_main()
{
	terminal_initialize();
	terminal_print("Hello World!\n");

	kheap_init();

	fs_init();

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

	struct path_root *root_path = pathparser_parse("0:/bin/shell.exe", NULL);
	if (!root_path)
		return;

	terminal_print_num(root_path->drive_no);
	struct path_part *path_part = root_path->first;
	while (path_part) {
		terminal_print_endl(path_part->part);
		path_part = path_part->next;
	}

	// struct disk_stream *stream = diskstream_new(0);
	// diskstream_seek(stream, 0x201);
	// unsigned char c = 0;
	// diskstream_read(stream, &c, 1);
	// while(1) {}

	char buf[30];
	strcpy(buf, "Hello World!");
	terminal_print_endl(buf);

	int fd = fopen("0:/hello.txt", "r");
	terminal_print_num(fd);
	if (fd)
		terminal_print_endl("We opened /hello.txt");

	fd = fopen("0:/home/hello_2.txt", "r");
	terminal_print_num(fd);
	if (fd)
		terminal_print_endl("We opened /home/hello_2.txt");

	int res = 0;
	res = fseek(fd, 2, SEEK_SET);
	if (res < 0)
		terminal_print_endl("seek set fail");
	res = fseek(fd, 1, SEEK_CUR);
	if (res < 0)
		terminal_print_endl("seek cur fail");
	res = fread(buf, 20, 1, fd);
	if (res < 0)
		terminal_print_endl("fread fail");
	buf[11] = 0x00;
	terminal_print_endl(buf);
	struct file_stat stat;
	res = fstat(fd, &stat);
	if (res < 0)
		terminal_print_endl("fstat fail");
	terminal_print_num(stat.filesize);
	terminal_print_num(stat.flags);

	terminal_print_endl("end");
}