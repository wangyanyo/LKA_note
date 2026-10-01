#ifndef __KERNEL_STRING_H
#define __KERNEL_STRING_H

#include <stddef.h>

int strlen(const char *str);
int strnlen(const char *str, int max_len);
char *strcpy(char *dst, const char *str);
char *strncpy(char *dst, const char *str, int n);
char tolower(char c);
int strnlen_terminator(const char *str, int max_len, char terminator);
int strncmp(const char *s1, const char *s2, int n);
int istrncmp(const char *s1, const char *s2, int n);
int isdigit(char c);
int tonumericdigit(char c);
void *memset(void *ptr, char c, size_t size);
int memcmp(void *p1, void *p2, size_t size);
void* memcpy(void* dest, const void* src, unsigned n);

#endif