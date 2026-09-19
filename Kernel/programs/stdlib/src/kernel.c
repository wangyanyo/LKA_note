#include "kernel.h"

int kernel_getkeyblock()
{
	int val = 0;
	do {
		val = kernel_getkey();
	} while(val == 0);
	return val;
}

void kernel_terminal_readline(char *out, int max, int output_while_typing)
{
	int i = 0, j;
	int key;

	while (i < max) {
		key = kernel_getkeyblock();
		if (key == 13) {
			break;
		}

		if (key == 0x08) {
			if (i >= 1 && output_while_typing)
				kernel_putchar(key);
			j = !i ? 0 : i - 1;
			out[j] = 0x00;
			i = j;
			continue;
		}

		if (output_while_typing)
			kernel_putchar(key);

		out[i++] = key;
	}
	out[i] = 0x00;
}