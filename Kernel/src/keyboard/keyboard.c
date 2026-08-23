#include "keyboard.h"
#include "status.h"
#include "task/process.h"
#include "task/task.h"

static struct keyboard *keyboard_list_head;
static struct keyboard *keyboard_list_tail;

void keyboard_init()
{

}

int keyboard_insert(struct keyboard *keyboard)
{
	int res = 0;

	if (!keyboard || !keyboard->init) {
		res = -EINVAGS;
		goto out;
	}

	if (!keyboard_list_head) {
		keyboard_list_head = keyboard;
		keyboard_list_tail = keyboard;
	} else {
		keyboard_list_tail->next = keyboard;
		keyboard_list_tail = keyboard;
	}

	res = keyboard->init();

out:
	return res;
}

static int keyboard_get_next_index(int index)
{
	return (index + 1) % KERNEL_KEYBOARD_BUFFER_SIZE;
}

static int keyboard_get_last_index(int index)
{
	return (index + KERNEL_KEYBOARD_BUFFER_SIZE - 1) % KERNEL_KEYBOARD_BUFFER_SIZE;
}

void keyboard_push(char c)
{
	struct process *process = process_current();
	if (!process)
		return;
	
	if (process->keyboard.size == KERNEL_KEYBOARD_BUFFER_SIZE)
		return;

	int index = keyboard_get_next_index(process->keyboard.tail);
	process->keyboard.buffer[index] = c;
	process->keyboard.tail = index;
	process->keyboard.size++;
}

char keyboard_pop()
{
	struct process *process = process_current();
	if (!process)
		return 0x00;

	if (process->keyboard.size == 0)
		return 0x00;

	int index = keyboard_get_next_index(process->keyboard.head);
	char c = process->keyboard.buffer[index];
	process->keyboard.buffer[index] = 0x00;
	process->keyboard.head = index;
	process->keyboard.size--;
	return c;
}
void keyboard_backspace(struct process *process)
{
	if (!process)
		return;

	if (process->keyboard.size == 0)
		return;
	
	process->keyboard.buffer[process->keyboard.tail] = 0x00;
	int index = keyboard_get_last_index(process->keyboard.tail);
	process->keyboard.tail = index;
	process->keyboard.size--;
}