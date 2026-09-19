#ifndef __TASK_PROCESS_H
#define __TASK_PROCESS_H

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

struct command_arugment {
	char argument[512];
	struct command_arugment *next;
};

struct process_arugment {
	int argc;
	char **argv;
};

struct process {
	/* The process id */
	uint16_t id;

	char filename[KERNEL_MAX_PATH];

	/* The main process task */
	struct task *task;

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

	struct process_arugment argument;
};

struct process *process_get(int process_id);
struct process *process_current();
int process_load_for_slot(char *filename, struct process **process, int process_slot);
int process_load(char *filename, struct process **process);
int process_load_switch(char *filename, struct process **process);
void *paging_align_address(void *ptr);
void *process_malloc(struct process *process, size_t size);
void process_free(struct process *process, void *ptr);
void process_get_arugment(struct process *process, int *argc, char ***argv);
int process_inject_argument(struct process *process, struct command_arugment *root_argument);

#endif