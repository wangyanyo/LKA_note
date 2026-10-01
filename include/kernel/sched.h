#ifndef __KERNEL_SCHED_H
#define __KERNEL_SCHED_H

#include "task.h"
#include "config.h"

typedef enum {
	PROCESS_FILETYPE_ELF,
	PROCESS_FILETYPE_BINARY,
} PROCESS_FILETYPE;

// 这里已经有VA_area的雏形了
struct process_allocation {
	void *ptr;
	size_t size;
};

struct command_argument {
	char argument[512];
	struct command_argument *next;
};

struct process_argument {
	int argc;
	char **argv;
};

struct task_struct {
	/* The process id */
	uint16_t id;

	char filename[KERNEL_MAX_PATH];

	/* The main process task */
	struct thread_info *task;

	/* The memory (malloc) allocations of the process */
	struct process_allocation allocations[KERNEL_MAX_PROGRAM_ALLOCATIONS];

	PROCESS_FILETYPE filetype;
	union {
		void *ptr;
		struct elf_file *elf_file;
	};

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

	struct process_argument argument;

	/* 进程返回值 */
	int res;
};

struct task_struct *process_get(int process_id);
struct task_struct *process_current();
int process_load_for_slot(char *filename, struct task_struct **process, int process_slot);
int process_load(char *filename, struct task_struct **process);
int process_load_switch(char *filename, struct task_struct **process);
void *paging_align_address(void *ptr);
void *process_malloc(struct task_struct *process, size_t size);
void process_free(struct task_struct *process, void *ptr);
void process_get_argument(struct task_struct *process, int *argc, char ***argv);
int process_inject_argument(struct task_struct *process, struct command_argument *root_argument);
int process_terminate(struct task_struct *process);
int process_switch(struct task_struct *process);

#endif