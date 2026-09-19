#include "shell.h"
#include "stdlib.h"
#include "stdio.h"
#include "kernel.h"

int main(int argc, char **argv)
{
	while (1) {
		print(">");
		char buf[1024];
		kernel_terminal_readline(buf, 1024, 1);
		print("\n");

		print(buf); print("\n");	
	}
	return 0;
}