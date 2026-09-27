#include "kernel.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "memory.h"

int main(int argc, char **argv)
{
	print("Hello World!\n");

	for (int i = 0; i < argc; ++i) {
		printf("%s\n", argv[i]);
	}

	while(1) {}
	return 0;
}