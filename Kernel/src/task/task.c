#include "task/task.h"
#include "memory/heap/kheap.h"
#include "status.h"
#include "terminal/print.h"

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
	paging_switch(task->page_directory->directory_entry);
	return 0;
}

int task_page(struct task *task)
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