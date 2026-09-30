#ifndef __FS_PPARSER_H
#define __FS_PPARSER_H

struct path_root {
        int drive_no;
        struct path_part *first;
};

struct path_part {
        char *part;
        struct path_part *next;
};

struct path_root *pathparser_parse(const char *path, const char *current_path);
void pathparser_free(struct path_root *root);

#endif