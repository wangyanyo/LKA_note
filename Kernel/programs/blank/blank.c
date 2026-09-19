#include "kernel.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "memory.h"

int main(int argc, char **argv)
{
	struct process_argument argument;
	kernel_process_get_argument(&argument);

	printf("%i, %s\n", argument.argc, argument.argv[0]);

	while(1) {}
	return 0;
}