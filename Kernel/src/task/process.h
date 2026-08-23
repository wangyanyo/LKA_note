#ifndef __TASK_PROCESS_H
#define __TASK_PROCESS_H

#include "task.h"
#include "config.h"

struct process {
	/* The process id */
	uint16_t id;

	char filename[KERNEL_MAX_PATH];

	/* The main process task */
	struct task *task;

	/* The memory (malloc) allocations of the process */
	void *allocations[KERNEL_MAX_PROGRAM_ALLOCATIONS];

	/* The physical pointer to the process memory */
	void *ptr;

	/* The physical pointer to the stack memory */
	void *stack;

	/* The size of the data pointed by 'ptr' */
	uint32_t size;

	struct keyboard_buffer {
		char buffer[KERNEL_KEYBOARD_BUFFER_SIZE];
		int head;
		int tail;
		int size;
	} keyboard;
};

struct process *process_get(int process_id);
struct process *process_current();
int process_load_for_slot(char *filename, struct process **process, int process_slot);
int process_load(char *filename, struct process **process);
void *paging_align_address(void *ptr);

#endif