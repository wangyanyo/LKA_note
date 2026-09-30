#ifndef __STDLIB_SRC_STDLIB_H
#define __STDLIB_SRC_STDLIB_H

#include <stddef.h>

void *malloc(size_t size);
void free(void *ptr);

char *itoa(int val);

#endif