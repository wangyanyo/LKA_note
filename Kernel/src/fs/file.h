#ifndef __FS_FILE_H
#define __FS_FILE_H

#include "pparser.h"

typedef unsigned int FILE_SEEK_MODE;
enum {
        SEEK_SET,
        SEEK_CUR,
        SEEK_END
};

typedef unsigned int FILE_MODE;
enum {
        FILE_MODE_READ,
        FILE_MODE_WRITE,
        FILE_MODE_APPEND,
        FILE_MODE_INVALID
};

struct disk;
typedef void *(*FS_OPEN_FUNCTION)(struct disk *disk, struct path_part *path, FILE_MODE mode);
typedef int (*FS_RESOLVE_FUNCTION)(struct disk *disk);

struct filesystem {
        char name[20];
        FS_RESOLVE_FUNCTION resolve;
        FS_OPEN_FUNCTION open;
};

struct file_descriptor {
        int index;
        struct filesystem *filesystem;
        void *private;
        struct disk *disk;
};

void fs_init();
void fs_insert_filesystem(struct filesystem *filesystem);
struct filesystem *fs_resolve(struct disk *disk);
int fopen(const char* filename, const char* mode);

#endif