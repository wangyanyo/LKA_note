#include "kernel/pparser.h"
#include "kernel/config.h"
#include "kernel/status.h"
#include "kernel/kheap.h"
#include "kernel/string.h"
#include "kernel/print.h"

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
        struct path_root *path_root = kzalloc(sizeof(struct path_root));
        if (!path_root) 
                return ERROR(-ENOMEM);
        path_root->drive_no = drive_no;
        path_root->first = NULL;
        return path_root;
}

static char *pathparser_get_path_part(const char **path)
{
        int i = 0;
        char *path_part_str = NULL;

        path_part_str = kzalloc(KERNEL_MAX_PATH);
        if (!path_part_str) {
                path_part_str = ERROR(-ENOMEM);
                goto out;
        }

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
        if (IS_ERROR(path_part_str)) {
                path_part = ERROR(path_part_str);
                goto out;
        }

        path_part = kzalloc(sizeof(struct path_part));
        if (!path_part) {
                kfree(path_part_str);
                path_part = ERROR(-ENOMEM);
                goto out;
        }

        path_part->part = path_part_str;
        path_part->next = NULL;

        if (last_part)
                last_part->next = path_part;

out:
        return path_part;
}

struct path_root *pathparser_parse(const char *path, const char *current_directory_path)
{
        int ret = 0;
        int drive_no = 0;
        const char *tmp_path = path;
        struct path_root *path_root = NULL;
        struct path_part *path_part = NULL;

        if (strlen(path) > KERNEL_MAX_PATH) {
                ret = -EINVAGS;
                goto out;
        }

        drive_no = pathparser_get_drive_by_path(&tmp_path);
        if (drive_no < 0) {
                ret = -EINVAGS;
                goto out;
        }

        path_root = pathparser_create_root(drive_no);
        if (IS_ERROR(path_root)) {
                ret = ERROR_I(path_root);
                goto out;
        }

        path_part = pathparser_parse_path_part(NULL, &tmp_path);
        if (IS_ERROR(path_part) || !path_part) {
                ret = IS_ERROR(path_part) ? ERROR_I(path_part) : -EINVAGS;
                goto out;
        }
        
        path_root->first = path_part;
        do {
                path_part = pathparser_parse_path_part(path_part, &tmp_path);
                if (IS_ERROR(path_part)) {
                        ret = ERROR_I(path_part);
                        goto out;
                }
        } while (path_part);

out:
        if (ret < 0) {
                if (path_root && !IS_ERROR(path_root))
                        pathparser_free(path_root);
                path_root = ERROR(ret);
        }

        return path_root;
}

void pathparser_free(struct path_root *path_root)
{
        struct path_part *path_part = NULL;

        if (!path_root)
                return;

        path_part = path_root->first;
        while (path_part) {
                struct path_part *next_part = path_part->next;
                kfree((void *)path_part->part);
                kfree(path_part);
                path_part = next_part;
        }
        kfree(path_root);
}