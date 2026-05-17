#ifndef DISK_H
#define DISK_H

typedef unsigned int KERNEL_DISK_TYPE;

#define KERNEL_DISK_TYPE_REAL 0

#include "fs/file.h"

// 目前disk的描述信息还很少
struct disk {
        KERNEL_DISK_TYPE type;
        int sector_size;
        struct filesystem *filesystem;
};

void disk_search_and_init();
struct disk *disk_get(int index);
int disk_read_block(struct disk *idisk, unsigned int lba, int total, void* buf);

#endif