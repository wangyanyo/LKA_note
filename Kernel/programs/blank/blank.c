#include "kernel.h"

int main(int argc, char **argv)
{
	print("I print something by C\n");
	while(1) {
		if (getkey() != 0) {
			print("key was pressed!\n");
		}
	}
	return 0;
}