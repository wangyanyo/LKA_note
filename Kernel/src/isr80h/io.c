#include "io.h"
#include "task/task.h"
#include "terminal/print.h"

void *isr80h_command0_print(struct interrupt_frame *frame)
{
	char *user_string_virt_ptr = task_get_stack_item(task_current(), 0);
	char buf[1024];
	copy_string_from_task(task_current(), user_string_virt_ptr, buf, sizeof(buf));
	terminal_print_endl(buf);
	return 0;
}