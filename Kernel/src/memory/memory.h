#ifndef __MEMORY_MEMORY_H
#define __MEMORY_MEMORY_H

#include <stddef.h>

void *memset(void *ptr, char c, size_t size);
int memcmp(void *p1, void *p2, size_t size);
void* memcpy(void* dest, const void* src, unsigned n);

#endif