#include "kernel.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "memory.h"

int main(int argc, char **argv)
{
	printf("%i, %s\n", argc, argv[0]);
	print("Hello World!\n");

	while(1) {}
	return 0;
}