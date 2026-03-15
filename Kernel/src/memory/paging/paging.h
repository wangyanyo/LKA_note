#ifndef __PAGING_PAGING_H
#define __PAGING_PAGING_H

#include <stdint.h>
#include <stddef.h>

#define PAGING_CACHE_DISABLED  0b00010000
#define PAGING_WRITE_THROUGH   0b00001000
#define PAGING_ACCESS_FROM_ALL 0b00000100
#define PAGING_IS_WRITEABLE    0b00000010
#define PAGING_IS_PERSENT      0b00000001

#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024
#define PAGING_PAGE_SIZE 4096

struct paging_4gb_trunk {
        uint32_t *directory_entry;
};

struct paging_4gb_trunk *paging_new_4gb_trunk(uint8_t flag);
void paging_switch(uint32_t *directory);
uint32_t *paging_4gb_trunk_get_directory(struct paging_4gb_trunk *trunk);
void enable_paging();

#endif