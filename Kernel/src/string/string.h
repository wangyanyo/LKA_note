#ifndef __STRING_STRING_H
#define __STRING_STRING_H

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

#endif