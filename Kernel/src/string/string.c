#include "string.h"

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
	char *res = dst;
	for (int i = 0; i < n && str[i]; ++i)
		dst[i] = str[i];
	if (n > 0)
		dst[n - 1] = 0x00;
	return dst;
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