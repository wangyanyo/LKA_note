#include "memory.h"
#include "status.h"

void *memset(void *ptr, char c, size_t size)
{
        char* c_ptr = (char*)ptr;
        for (size_t i = 0; i < size; ++i) {
                c_ptr[i] = c;
        }
        return ptr;
}

int memcmp(void *p1, void *p2, size_t size)
{
        char *s1 = p1;
        char *s2 = p2;
        for (size_t i = 0; i < size; ++i) {
                if (s1[i] != s2[i])
                        return s1[i] < s2[i] ? -1 : 1;
        }
        return 0;
}

void* memcpy(void* dest, const void* src, unsigned n)
{
	int i;
	char *d = (char *)dest, *s = (char *)src;

	for (i=0;i<n;i++) d[i] = s[i];
	return dest;
}