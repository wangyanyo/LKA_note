#include "file.h"
#include "config.h"
#include "status.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "terminal/print.h"
#include "fat/fat16.h"
#include "disk/disk.h"
#include "string/string.h"

struct filesystem *filesystems[KERNEL_MAX_FILESYSTEMS];
struct file_descriptor *file_descriptors[KERNEL_MAX_FILE_DESCRIPTORS];

static struct filesystem **fs_get_free_filesystem()
{
	int i = 0;
	// 这里应该加锁保护
	for (i = 0; i < KERNEL_MAX_FILESYSTEMS; ++i)
		if (!filesystems[i])
			return &filesystems[i];
	return NULL;
}

void fs_insert_filesystem(struct filesystem *filesystem)
{
	struct filesystem **fs;
	fs = fs_get_free_filesystem();
	if (!fs) {
		// Panic
		terminal_print("Problem inserting filesystem");
		while (1) {}
	}

	*fs = filesystem;
}

static void fs_static_load()
{
	fs_insert_filesystem(fat16_init());
}

void fs_init()
{
	// 这种初始化函数需要锁保护吗？我觉得应该不用，内核初始化流程是单核且不支持抢占的
	memset(filesystems, 0x00, sizeof(filesystems));
	fs_static_load();
}

struct filesystem *fs_resolve(struct disk *disk)
{
	int i = 0;
	// 这里应该用锁保护起来
	for (i = 0; i < KERNEL_MAX_FILESYSTEMS; ++i)
		if (filesystems[i] != NULL && filesystems[i]->resolve(disk) == 0)
			return filesystems[i];
	return NULL;
}

static int file_new_descriptor(struct file_descriptor** desc_out)
{
	int i = 0;
	int res = -ENOMEM;
	// 这里应该用锁保护起来
	for (i = 0; i < KERNEL_MAX_FILE_DESCRIPTORS; ++i) {
		if (!file_descriptors[i]) {
			struct file_descriptor* desc = kzalloc(sizeof(struct file_descriptor));
			file_descriptors[i] = desc;
			desc->index = i + 1;
			*desc_out = desc;
			res = 0;
			break;
		}
	}
	return res;
}

static struct file_descriptor *file_get_descriptor(int fd)
{
	if (fd <= 0 || fd > KERNEL_MAX_FILE_DESCRIPTORS)
		return NULL;

	return file_descriptors[fd - 1];
}

static FILE_MODE file_get_mode_by_string(const char *str)
{
	FILE_MODE mode = FILE_MODE_INVALID;
	if (strncmp(str, "r", 1) == 0)
		mode = FILE_MODE_READ;
	else if (strncmp(str, "w", 1) == 0)
		mode = FILE_MODE_WRITE;
	else if (strncmp(str, "a", 1) == 0)
		mode = FILE_MODE_APPEND;
	return mode;
}

int fopen(const char* filename, const char* mode_str)
{
	int res = 0;
	struct path_root *root_path = pathparser_parse(filename, NULL);
	if (!root_path || !root_path->first) {
		res = -EINVAGS;
		goto out;
	}

	struct disk *disk = disk_get(root_path->drive_no);
	if (!disk || !disk->filesystem) {
		res = -EIO;
		goto out;
	}

	FILE_MODE mode = file_get_mode_by_string(mode_str);
	if (mode == FILE_MODE_INVALID) {
		res = -EINVAGS;
		goto out;
	}

	void *descriptor_private_data = disk->filesystem->open(disk, root_path->first, mode);
	if (IS_ERROR(descriptor_private_data)) {
		res = ERROR_I(descriptor_private_data);
		goto out;
	}

	struct file_descriptor *desc = NULL;
	res = file_new_descriptor(&desc);
	if (res < 0)
		goto out;

	desc->disk = disk;
	desc->filesystem = disk->filesystem;
	desc->private = descriptor_private_data;
	res = desc->index;
out:
	if (res < 0)
		res = 0;

	return res;
}

int fread(void *ptr, uint32_t size, uint32_t nmemb, int fd)
{
	int res = 0;
	struct file_descriptor *desc = NULL;

	if (size == 0 || nmemb == 0 || fd < 1) {
		res = -EINVAGS;
		goto out;
	}

	desc = file_get_descriptor(fd);
	if (!desc) {
		res = -EINVAGS;
		goto out;
	}

	res = desc->filesystem->read(desc->disk, desc->private, size, nmemb, ptr);

out:
	return res;
}

int fseek(int fd, int offset, FILE_SEEK_MODE whence)
{
	int res = 0;
	struct file_descriptor *desc = NULL;

	desc = file_get_descriptor(fd);
	if (!desc) {
		res = -EINVAGS;
		goto out;
	}

	res = desc->filesystem->seek(desc->private, offset, whence);

out:
	return res;
}

int fstat(int fd, struct file_stat *stat)
{
	int res = 0;
	struct file_descriptor *desc = NULL;

	desc = file_get_descriptor(fd);
	if (!desc) {
		res = -EINVAGS;
		goto out;
	}

	res = desc->filesystem->stat(desc->disk, desc->private, stat);

out:
	return res;
}