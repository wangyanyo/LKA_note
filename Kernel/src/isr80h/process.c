#include "process.h"
#include "task/process.h"
#include "task/task.h"

void *isr80h_command6_process_load_start(struct interrupt_frame *frame)
{
	int ret = 0;
	char *user_data_ptr = NULL;
	char filepath[KERNEL_MAX_PATH] = "0:/";
	struct process *process = NULL;

	user_data_ptr = task_get_stack_item(task_current(), 0);
	ret = copy_string_from_task(task_current(), user_data_ptr, filepath + 3, KERNEL_MAX_PATH - 3);
	if (ret < 0)
		goto out;

	ret = process_load_switch(filepath, &process);
	if (ret < 0)
		goto out;
	
	task_switch(process->task);
	task_return(&process->task->registers);

out:
	return (void *)ret;
}