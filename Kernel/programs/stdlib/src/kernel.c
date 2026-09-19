#include "kernel.h"
#include "string.h"

struct command_argument* kernel_parse_command(const char* command, int max)
{
	struct command_argument* root_command = 0;
	char scommand[1024];
	if (max >= (int) sizeof(scommand))
		return 0;


	strncpy(scommand, command, sizeof(scommand));
	char* token = strtok(scommand, " ");
	if (!token)
		goto out;

	root_command = kernel_malloc(sizeof(struct command_argument));
	if (!root_command)
		goto out;

	strncpy(root_command->argument, token, sizeof(root_command->argument));
	root_command->next = 0;


	struct command_argument* current = root_command;
	token = strtok(NULL, " ");
	while(token != 0) {
		struct command_argument* new_command = kernel_malloc(sizeof(struct command_argument));
		if (!new_command)
			break;

		strncpy(new_command->argument, token, sizeof(new_command->argument));
		new_command->next = 0x00;
		current->next = new_command;
		current = new_command;
		token = strtok(NULL, " ");
	}
out:
	return root_command;
}

int kernel_getkeyblock()
{
	int val = 0;
	do {
		val = kernel_getkey();
	} while(val == 0);
	return val;
}

void kernel_terminal_readline(char *out, int max, int output_while_typing)
{
	int i = 0, j;
	int key;

	while (i < max) {
		key = kernel_getkeyblock();
		if (key == 13) {
			break;
		}

		if (key == 0x08) {
			if (i >= 1 && output_while_typing)
				kernel_putchar(key);
			j = !i ? 0 : i - 1;
			out[j] = 0x00;
			i = j;
			continue;
		}

		if (output_while_typing)
			kernel_putchar(key);

		out[i++] = key;
	}
	out[i] = 0x00;
}