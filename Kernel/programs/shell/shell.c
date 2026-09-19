#include "shell.h"
#include "stdlib.h"
#include "stdio.h"
#include "kernel.h"

int main(int argc, char **argv)
{
	int ret;

	while (1) {
		print(">");
		char buf[1024];
		kernel_terminal_readline(buf, 1024, 1);
		print("\n");

		ret = kernel_process_load_start(buf);
		if (ret < 0) {
			printf("exec %s fail: %i\n", buf, ret);
		}
	}
	return 0;
}