#ifndef __TERMINAL_PRINT_H
#define __TERMINAL_PRINT_H

#include <stdint.h>
#include <stddef.h>

void terminal_initialize();

void terminal_print(const char* str);

void terminal_print_num(unsigned long num);

void terminal_print_hex(unsigned long num);

#endif