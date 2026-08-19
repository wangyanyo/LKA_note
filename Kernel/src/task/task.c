#include "task/task.h"
#include "memory/heap/kheap.h"
#include "status.h"
#include "terminal/print.h"
#include "idt/idt.h"
#include "string/string.h"
#include "kernel.h"

struct task *current_task = 0;

struct task *task_head = 0;
struct task *task_tail = 0;

/* 这里代码冗余，后续用嵌入式链表改造 */
static void task_insert(struct task *task)
{
	if (!task_head) {
		task_head = task;
		task_tail = task;
		current_task = task;
		return;
	}

	task_tail->next = task;
	task->prev = task_tail;
	task->next = NULL;
	task_tail = task;
}

static int task_init(struct task *task, struct process *process)
{
	task->page_directory = paging_new_4gb_chunk(PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
	if (!task->page_directory)
		return -EIO;

	task->registers.ip = KERNEL_PROGRAM_VIRTUAL_ADDRESS;
	task->registers.ss = USER_DATA_SEGMENT;
	task->registers.cs = USER_CODE_SEGMENT;
	task->registers.esp = KERNEL_PROGRAM_VIRTUAL_STACK_ADDRESS_START;

	task->process = process;

	return 0;
}

/* 这个链表维护起来太麻烦了，以后要换成嵌入式链表 */
static void task_list_remove(struct task *task)
{
	if (task_head == task) {
		task_head = task->next;
	}

	if (task_tail == task) {
		task_tail = task->prev;
	}

	if (current_task == task) {
		current_task = task_get_next(current_task);
	}

	if (task->prev) {
		task->prev->next = task->next;
	}

	if (task->next) {
		task->next->prev = task->prev;
	}
}

struct task *task_new(struct process *process)
{
	int res = 0;
	struct task *task = NULL;

	task = kzalloc(sizeof(struct task));
	if (!task) {
		res = -ENOMEM;
		goto out;
	}

	res = task_init(task, process);
	if (res != KERNEL_ALL_OK)
		goto out;

	task_insert(task);

out:
	if (IS_ERROR(res)) {
		task_free(task);
		return ERROR(task);
	}

	return task;
}

struct task *task_current()
{
	return current_task;
}

struct task *task_get_next()
{
	if (!current_task || !current_task->next)
		return task_head;
	return current_task->next;
}

int task_free(struct task *task)
{
	paging_free_4gb(task->page_directory);
	task_list_remove(task);

	kfree(task);
	return 0;
}

int task_switch(struct task *task)
{
	current_task = task;
	paging_switch(task->page_directory);
	return 0;
}

int task_page()
{
	user_registers();
	task_switch(current_task);
	return 0;
}

int task_page_task(struct task *task)
{
	user_registers();
	task_switch(task);
	return 0;
}

int task_run_first_ever_task()
{
	if (!current_task)
		panic("task_run_first_ever_task: No current task exists!\n");

	task_switch(task_head);
	task_return(&task_head->registers);
	return 0;
}

static void task_save_state(struct task *task, struct interrupt_frame *frame)
{
	task->registers.ip = frame->ip;
	task->registers.cs = frame->cs;
	task->registers.flags = frame->flags;
	task->registers.esp = frame->esp;
	task->registers.ss = frame->ss;
	task->registers.eax = frame->eax;
	task->registers.ebp = frame->ebp;
	task->registers.ebx = frame->ebx;
	task->registers.ecx = frame->ecx;
	task->registers.edi = frame->edi;
	task->registers.edx = frame->edx;
	task->registers.esi = frame->esi;
}

void task_current_save_state(struct interrupt_frame *frame)
{
	if (!task_current())
		panic("No current task to save");
	
	struct task *task = task_current();
	task_save_state(task, frame);
}

int copy_string_from_task(struct task *task, void *virtual, void *phys, int max)
{
	int res = 0;

	if (!task || !virtual || !phys) {
		res = -EINVAGS;
		goto out;
	}

	if (max < 0 || max > PAGING_PAGE_SIZE) {
		res = -EINVAGS;
		goto out;
	}

	void *tmp = kzalloc(max);
	if (!tmp) {
		res = -ENOMEM;
		goto out;
	}

	uint32_t old_entry = paging_get(task->page_directory, virtual);
	res = paging_map(task->page_directory, tmp, tmp, PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
	if (res != KERNEL_ALL_OK)
		goto free_out;
	paging_switch(task->page_directory);
	strncpy(tmp, virtual, max);
	kernel_page();

	res = paging_set(task->page_directory->directory_entry, tmp, old_entry);
	if (res != KERNEL_ALL_OK)
		goto free_out;

	strncpy(phys, tmp, max);

free_out:
	kfree(tmp);
out:
	return res;

}

void *task_get_stack_item(struct task *task, int index)
{
	void *result = 0;
	uint32_t *sp_ptr = (uint32_t *)task->registers.esp;
	task_page_task(task);
	result = (void *)sp_ptr[index];
	kernel_page();
	return result;
}