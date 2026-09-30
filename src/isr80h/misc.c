#include "misc.h"
#include "task/task.h"

void *isr80h_command0_sum(struct interrupt_frame *frame)
{
	uint32_t item_1 = (uint32_t)task_get_stack_item(task_current(), 0);
	uint32_t item_2 = (uint32_t)task_get_stack_item(task_current(), 1);
	return (void *)(item_1 + item_2);
}