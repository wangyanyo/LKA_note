#include "pparser.h"
#include "config.h"
#include "status.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "string/string.h"

static int pathpraser_path_vaild_format(const char *path)
{
        int len = strnlen(path, KERNEL_MAX_PATH);
        return (len >= 3 && isdigit(path[0]) && memcmp((void*)&path[1], ":/", 2) == 0);
}

static int pathparser_get_drive_by_path(const char **path)
{
        int drive_no = -1;

        if (!pathpraser_path_vaild_format(*path))
                return -EBADPATH;
        
        drive_no = tonumericdigit(*path[0]);

        *path += 3;
        return drive_no;
}

static struct path_root *pathparser_create_root(int drive_no)
{
        struct path_root *path_root = kmalloc(sizeof(struct path_root));
        path_root->drive_no = drive_no;
        path_root->first = NULL;
        return path_root;
}

static char *pathparser_get_path_part(const char **path)
{
        int i = 0;
        char *path_part_str = NULL;

        path_part_str = kmalloc(KERNEL_MAX_PATH);
        if (!path_part_str)
                goto out;

        while (**path != '/' && **path != 0x00) {
                path_part_str[i++] = **path;
                *path += 1;
        }

        if (**path == '/')
                *path += 1;

	path_part_str[i] = 0;

        if (i == 0) {
                kfree(path_part_str);
                path_part_str = NULL;
                goto out;
        }

out:
        return path_part_str;
}

static struct path_part *pathparser_parse_path_part(struct path_part *last_part, const char **path)
{
        struct path_part *path_part = NULL;
        char *path_part_str = NULL;

        path_part_str = pathparser_get_path_part(path);
        if (!path_part_str)
                goto out;

        path_part = kzalloc(sizeof(struct path_part));
        if (!path_part)
                goto out;

        path_part->part = path_part_str;
        path_part->next = NULL;

        if (last_part)
                last_part->next = path_part;

out:
        return path_part;
}

struct path_root *pathparser_parse(const char *path, const char *current_directory_path)
{
        int drive_no = 0;
        const char *tmp_path = path;
        struct path_root *path_root = NULL;
        struct path_part *path_part = NULL;

        if (strlen(path) > KERNEL_MAX_PATH)
                goto out;

        drive_no = pathparser_get_drive_by_path(&tmp_path);
        if (drive_no < 0)
                goto out;

        path_root = pathparser_create_root(drive_no);
        if (!path_root)
                goto out;

        path_part = pathparser_parse_path_part(NULL, &tmp_path);
        if (!path_part)
                goto out;
        
        path_root->first = path_part;
        do {
                path_part = pathparser_parse_path_part(path_part, &tmp_path);
        } while (path_part);

out:
        return path_root;
}

void pathparser_free(struct path_root *path_root)
{
        struct path_part *path_part = path_root->first;
        while (path_part) {
                struct path_part *next_part = path_part->next;
                kfree((void *)path_part->part);
                kfree(path_part);
                path_part = next_part;
        }
        kfree(path_root);
}