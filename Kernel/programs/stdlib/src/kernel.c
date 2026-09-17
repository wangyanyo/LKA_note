#include "kernel.h"

int kernel_getkeyblock()
{
	int val = 0;
	do {
		val = kernel_getkey();
	} while(val == 0);
	return val;
}

void kernel_readline(char *out, int max, int output_while_typing)
{
	int i;
	int key;

	for (i = 0; i < max; ++i) {
		key = kernel_getkeyblock();

		if (key == 13) {
			kernel_putchar('\n');
			break;
		}

		if (output_while_typing)
			kernel_putchar(key);
		
		if (key == 0x08 && i >= 1) {
			out[i] = 0x00;
			i -= 2;
			continue;
		}

		out[i] = key;
	}
	out[i] = 0x00;
}