#include "print.h"
#include "config.h"
#include "string/string.h"

#define DEFAULT_STRING_SIZE (KERNEL_CPU_BITS + 1)
#define NUM_CHAR_SET_SIZE 17

static const char num_char_set[NUM_CHAR_SET_SIZE] = "0123456789ABCDEF";

uint16_t* vedio_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

static uint16_t terminal_make_char(char c, char color)
{
	return (color << 8) | c;
}

static void terminal_put_char(int x, int y, char c, char color)
{
	vedio_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, color);
}

static void terminal_write_char(char c, char color)
{
	if (c == '\n') {
		terminal_row++;
		terminal_col = 0;
		return;
	}

	terminal_put_char(terminal_col, terminal_row, c, color);

	terminal_col++;
	if (terminal_col == VGA_WIDTH) {
		terminal_row++;
		terminal_col = 0;
	}
}

void terminal_initialize()
{
	vedio_mem = (uint16_t*)(0xB8000);
	for (int i = 0; i < VGA_HEIGHT; ++i) {
		for (int j = 0; j < VGA_WIDTH; ++j) {
			terminal_put_char(j, i, ' ', 0);
		}
	}
}

void terminal_print(const char* str)
{
	int len = strlen(str);
	for (int i = 0; i < len; ++i) {
		terminal_write_char(str[i], 15);
	}
}

void terminal_print_endl(const char* str)
{
	terminal_print(str);
	terminal_print("\n");
}

static void __ulong_to_string(unsigned long num, char *str, uint32_t size, uint32_t base)
{
	uint32_t index = 0;

	if (size == 0)
		goto out;

	if (num == 0) {
		str[index++] = '0';
		goto out;
	}

	while (num > 0) {
		uint32_t val = num % base;
		num /= base;
		str[index++] = num_char_set[val];
		if (index == size - 1) {
			goto out;
		}
	}

	for (int i = 0, j = index - 1; i < j; ++i, --j) {
		char tmp = str[i];
		str[i] = str[j];
		str[j] = tmp;
	}

out:
	str[index] = 0;
	return;
}

void ulong_to_string(unsigned long num, char *str, uint32_t size)
{
	__ulong_to_string(num, str, size, 10);
}

void ulong_to_hex_string(unsigned long num, char *str, uint32_t size)
{
	__ulong_to_string(num, str, size, 16);
}

void terminal_print_num(unsigned long num)
{
	char str[DEFAULT_STRING_SIZE];
	ulong_to_string(num, str, DEFAULT_STRING_SIZE);
	terminal_print(str);
	terminal_print("\n");
}

void terminal_print_hex(unsigned long num)
{
	char hex_str[DEFAULT_STRING_SIZE];
	ulong_to_hex_string(num, hex_str, DEFAULT_STRING_SIZE);
	terminal_print("0x");
	terminal_print(hex_str);
	terminal_print("\n");
}

void panic(char *msg)
{
	terminal_print_endl(msg);
	while(1) {}
}