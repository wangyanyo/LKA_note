#include "shell.h"
#include "stdlib.h"
#include "stdio.h"
#include "kernel.h"
#include "string.h"

int main(int argc, char **argv)
{
	int ret;

	while (1) {
		print(">");
		char buf[1024];
		kernel_terminal_readline(buf, 1024, 1);
		print("\n");

		if (!strlen(buf))
			continue;

		ret = kernel_system_run(buf);
		if (ret < 0) {
			printf("exec %s fail!\n", buf);
		}
	}
	return 0;
}