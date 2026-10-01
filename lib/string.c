#include "kernel/string.h"

char tolower(char c) {
        if (c >= 'A' && c <= 'Z')
                return c - 'A' + 'a';
        return c;
}

int strlen(const char *str)
{
        int len = 0;
        while (str[len] != 0x00)
                len++;
        return len;
}

int strnlen(const char *str, int max_len)
{
        int len = 0;
        while (len < max_len && str[len] != 0x00)
                len++;
        return len;
}

char *strcpy(char *dst, const char *str)
{
        char *res = dst;
        while (*str != 0) {
                *dst = *str;
                dst++;
                str++;
        }
        *dst = 0x00;
        return res;
}

char *strncpy(char *dst, const char *str, int n)
{
	int i = 0;
	char *res = dst;
	for (i = 0; i < n && str[i]; ++i)
		dst[i] = str[i];
	if (i > 0)
		dst[i] = 0x00;
	return res;
}

int strnlen_terminator(const char *str, int max_len, char terminator)
{
        int len = 0;
        while (len < max_len && str[len] != 0x00 && str[len] != terminator)
                len++;
        return len;
}

int strncmp(const char *s1, const char *s2, int n)
{
        for (int i = 0; i < n; ++i) {
                if (s1[i] != s2[i])
                        return s1[i] - s2[i];
                if (s1[i] == 0)
                        return 0;
        }
        return 0;
}

int istrncmp(const char *s1, const char *s2, int n)
{
        for (int i = 0; i < n; ++i) {
                char c1 = tolower(s1[i]);
                char c2 = tolower(s2[i]);
                if (c1 != c2)
                        return c1 - c2;
                if (c1 == 0)
                        return 0;
        }
        return 0;
}

int isdigit(char c)
{
        return (c >= '0' && c <= '9');
}

int tonumericdigit(char c)
{
        return c - '0';
}

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