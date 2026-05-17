#ifndef __STRING_STRING_H
#define __STRING_STRING_H

int strlen(const char *str);
int strnlen(const char *str, int max_len);
char *strcpy(char *dst, const char *str);
int isdigit(char c);
int tonumericdigit(char c);

#endif