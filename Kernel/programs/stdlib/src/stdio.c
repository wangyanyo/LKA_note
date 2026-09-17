#include "stdio.h"
#include "kernel.h"
#include "stdlib.h"
#include <stdarg.h>

void putchar(char c)
{
	kernel_putchar(c);
}

int printf(const char *fmt, ...)
{
	va_list ap;
	const char *p;
	int ival;
	char *sval;

	va_start(ap, fmt);
	for (p = fmt; *p; ++p) {
		if (*p != '%') {
			putchar(*p);
			continue;
		}
		
		switch (*++p) {
			case 'i':
				ival = va_arg(ap, int);
				print(itoa(ival));
				break;
			case 's':
				sval = va_arg(ap, char *);
				print(sval);
				break;
			default:
				putchar('%');
				putchar(*p);
				break;
		}
	}
	va_end(ap);
	return 0;
}