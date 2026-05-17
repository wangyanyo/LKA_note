#include "fat16.h"
#include "disk/disk.h"
#include "string/string.h"

int fat16_resolve(struct disk *disk);
void *fat16_open(struct disk *disk, struct path_part *path, FILE_MODE mode);

struct filesystem fat16_fs = {
        .open = fat16_open,
        .resolve = fat16_resolve,
};

struct filesystem *fat16_init()
{
        strcpy(fat16_fs.name, "FAT16");
        return &fat16_fs;
}

void *fat16_open(struct disk *disk, struct path_part *path, FILE_MODE mode)
{
        return NULL;
}

int fat16_resolve(struct disk *disk)
{
        return 0;
}