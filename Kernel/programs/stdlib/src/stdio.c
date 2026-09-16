#include "stdio.h"
#include "kernel.h"

void putchar(char c)
{
	kernel_putchar(c);
}