#include "string.h"

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

int isdigit(char c)
{
        return (c >= '0' && c <= '9');
}

int tonumericdigit(char c)
{
        return c - '0';
}