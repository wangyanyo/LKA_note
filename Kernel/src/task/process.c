#include "process.h"
#include "config.h"
#include "status.h"
#include "memory/heap/kheap.h"
#include "fs/file.h"
#include "memory/memory.h"
#include "string/string.h"

static struct process *processes[KERNEL_MAX_PROCESSES] = {};
static struct process *current_process;

struct process *process_get(int process_id)
{
	if (process_id < 0 || process_id >= KERNEL_MAX_PROCESSES)
		return NULL;
	
	return processes[process_id];
}

struct process *process_current()
{
	return current_process;
}

static void process_init(struct process *process)
{
	memset(process, 0x00, sizeof(struct process));
}

static int process_load_bin(char *filename, struct process *process)
{
	int res = 0;
	int fd;
	void *program_data_ptr = NULL;
	struct file_stat stat;

	fd = fopen(filename, "r");
	if (!fd) {
		res = -EIO;
		goto out;
	}

	res = fstat(fd, &stat);
	if (res != KERNEL_ALL_OK)
		goto out;

	program_data_ptr = kzalloc(stat.filesize);
	if (!program_data_ptr) {
		res = -ENOMEM;
		goto out;
	}

	if (fread(program_data_ptr, stat.filesize, 1, fd) != 1) {
		res = -EIO;
		goto out;
	}

	process->ptr = program_data_ptr;
	process->size = stat.filesize;

out:
	fclose(fd);
	return res;
}

/* 这里之所以又加了一层，是因为除了二进制文件外，我们还有可能记载ELF格式的文件 */
static int process_load_data(char *filename, struct process *process)
{
	int res = 0;
	res = process_load_bin(filename, process);
	return res;
}

static int process_map_binary(struct process *process)
{
	int res = 0;
	res = paging_map_to(process->task->page_directory->directory_entry, KERNEL_PROGRAM_VIRTUAL_ADDRESS, process->ptr,
		paging_align_address(process->ptr + process->size), PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);
	return res;
}

static int process_map_memory(struct process *process)
{
	int res = 0;
	res = process_map_binary(process);
	return res;
}

int process_load_for_slot(char *filename, struct process **process, int process_slot)
{
	int res = 0;
	struct task *task = NULL;
	struct process *_process = NULL;
	void *program_stack_ptr = NULL;

	/* 检查参数合法性 */
	if (!filename || !process || process_get(process_slot)) {
		res = -EINVAGS;
		goto out;
	}

	/* 创建process */
	_process = kzalloc(sizeof(struct process));
	if (!_process) {
		res = -ENOMEM;
		goto out;
	}
	process_init(_process);

	/* 读取文件内容到内存中，让ptr指向这块内存，size设置为文件大小 */
	res = process_load_data(filename, _process);
	if (res != KERNEL_ALL_OK)
		goto out;
	
	/* 申请一块栈内存，让stack指向这块内存 */
	program_stack_ptr = kzalloc(KERNEL_USER_PROGRAM_STACK_SIZE);
	if (!program_stack_ptr) {
		res = -ENOMEM;
		goto out;
	}
	_process->stack = program_stack_ptr;

	/* 创建一个task，并建立映射 */
	task = task_new();
	if (!task) {
		res = -ENOMEM;
		goto out;
	}
	res = process_map_memory(_process);
	if (res < 0)
		goto out;
	_process->task = task;

	/* 设置id，filename */
	_process->id = process_slot;
	strncpy(_process->filename, filename, sizeof(_process->filename));

	/* 收尾 */
	*process = _process;
	processes[process_slot] = _process;
out:
	if (IS_ERROR(res)) {
		if (_process && _process->task)
			task_free(_process->task);
	}

	return res;
}