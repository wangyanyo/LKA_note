#include "kernel.h"
#include "stdlib.h"
#include "stdio.h"

int main(int argc, char **argv)
{
	print("I print something by C\n");

	void *ptr = NULL;
	ptr = malloc(512);
	if (!ptr) {
		print("malloc fail\n");
		return -1;
	}
	free(ptr);

	print(itoa(123)); print("\n");
	print(itoa(0)); print("\n");
	print(itoa(-321)); print("\n");

	printf("My age is %i\n", 23);

	char buf[1024];
	kernel_readline(buf, 1024, 1);
	print(buf);

	return 0;
}