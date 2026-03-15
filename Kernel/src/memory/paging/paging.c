#include "paging.h"
#include "memory/heap/kheap.h"

extern void paging_load_directory(uint32_t *directory);

static uint32_t *current_directory = 0;

struct paging_4gb_trunk *paging_new_4gb_trunk(uint8_t flag)
{
        uint32_t *directory = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
        uint32_t offset = 0;
        for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; ++i) {
                uint32_t *entry = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
                for (int j = 0; j < PAGING_TOTAL_ENTRIES_PER_TABLE; ++j) {
                        entry[j] = (offset + j * PAGING_PAGE_SIZE) | flag;
                }
                offset = PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE;
                directory[i] = (uint32_t)entry | flag | PAGING_IS_WRITEABLE;
        }

        struct paging_4gb_trunk *trunk = kzalloc(sizeof(struct paging_4gb_trunk));

        trunk->directory_entry = directory;
        return trunk;
}

void paging_switch(uint32_t *directory)
{
        paging_load_directory(directory);
        current_directory = directory;
}

uint32_t *paging_4gb_trunk_get_directory(struct paging_4gb_trunk *trunk)
{
        return trunk->directory_entry;
}