#include "kernel.h"
#include "stdlib.h"

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

	while(1) {
		char key = getkey();
		if (key != 0) {
			putchar(key);
		}
	}
	return 0;
}