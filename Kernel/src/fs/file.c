#include "file.h"
#include "config.h"
#include "status.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "terminal/print.h"
#include "fat/fat16.h"

struct filesystem *filesystems[KERNEL_MAX_FILESYSTEMS];
struct file_descriptor *file_descriptors[KERNEL_MAX_FILE_DESCRIPTORS];

static struct filesystem **fs_get_free_filesystem()
{
        int i = 0;
        // 这里应该加锁保护
        for (i = 0; i < KERNEL_MAX_FILESYSTEMS; ++i) {
                if (!filesystems[i]) {
                        return &filesystems[i];
                }
        }
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
        for (i = 0; i < KERNEL_MAX_FILESYSTEMS; ++i) {
                if (filesystems[i] != NULL && filesystems[i]->resolve(disk) == 0) {
                        return filesystems[i];
                }
        }
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
        if (fd <= 0 || fd > KERNEL_MAX_FILE_DESCRIPTORS) {
                return NULL;
        }

        return file_descriptors[fd - 1];
}

int fopen(const char* filename, const char* mode)
{
        return -EIO;
}
