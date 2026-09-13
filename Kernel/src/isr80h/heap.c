#include "heap.h"
#include "task/process.h"

void *isr80h_command4_malloc(struct interrupt_frame *frame)
{
	void *item = task_get_stack_item(task_current(), 0);
	return process_malloc(task_current()->process, (size_t)item);
}