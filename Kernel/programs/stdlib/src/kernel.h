#ifndef __STDLIB_SRC_KERNEL_H
#define __STDLIB_SRC_KERNEL_H

#include <stddef.h>

void print(char *msg);
int getkey();
void kernel_putchar(char c);

void *kernel_malloc(size_t size);
void kernel_free(void *ptr);

#endif