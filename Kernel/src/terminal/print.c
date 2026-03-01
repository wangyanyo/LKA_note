#include "print.h"
#include "config.h"

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

static size_t strlen(const char* str)
{
	size_t len = 0;
	while (str[len]) {
		len++;
	}
	return len;
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
	size_t len = strlen(str);
	for (int i = 0; i < len; ++i) {
		terminal_write_char(str[i], 15);
	}
}