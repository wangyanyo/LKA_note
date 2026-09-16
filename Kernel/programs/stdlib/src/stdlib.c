#include "stdlib.h"
#include "kernel.h"

void *malloc(size_t size)
{
	return kernel_malloc(size);
}

void free(void *ptr)
{
	return kernel_free(ptr);
}

char *itoa(int val)
{
	static char text[12];
	int loc = 11;
	int neg = 0;
	
	text[11] = 0;
	if (val < 0) {
		neg = 1;
		val = -val;
	}

	while (val) {
		text[--loc] = (char)('0' + val % 10);
		val /= 10;
	}

	if (loc == 11)
		text[--loc] = '0';
	
	if (neg)
		text[--loc] = '-';

	return &text[loc];
}