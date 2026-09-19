#ifndef __STDLIB_SRC_KERNEL_H
#define __STDLIB_SRC_KERNEL_H

#include <stddef.h>

struct command_argument {
	char argument[512];
	struct command_argument* next;
};

struct process_argument {
	int argc;
	char **argv;
};

void print(char *msg);
int kernel_getkey();
void kernel_putchar(char c);

void *kernel_malloc(size_t size);
void kernel_free(void *ptr);

void kernel_terminal_readline(char *out, int max, int output_while_typing);
int kernel_process_load_start(const char *filename);
struct command_argument* kernel_parse_command(const char* command, int max);

void kernel_process_get_argument(struct process_argument *argument);

#endif