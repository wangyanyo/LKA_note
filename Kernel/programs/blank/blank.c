#include "kernel.h"
#include "stdlib.h"

int main(int argc, char **argv)
{
	print("I print something by C\n");

	void *ptr = NULL;
	ptr = malloc(512);
	if (!ptr) {
		print("malloc fail\n");
	}

	while(1) {
		char key = getkey();
		if (key != 0) {
			putchar(key);
		}
	}
	return 0;
}