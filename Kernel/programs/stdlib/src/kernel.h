#ifndef __STDLIB_SRC_KERNEL_H
#define __STDLIB_SRC_KERNEL_H

#include <stddef.h>

void print(char *msg);
int kernel_getkey();
void kernel_putchar(char c);

void *kernel_malloc(size_t size);
void kernel_free(void *ptr);

void kernel_readline(char *out, int max, int output_while_typing);

#endif