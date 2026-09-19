#include "kernel.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "memory.h"

int main(int argc, char **argv)
{
	char *ptr = malloc(20);
	strcpy(ptr, "Hello World!\n");
	print(ptr);

	free(ptr);

	print("abc\n");

	while(1) {}
	return 0;
}