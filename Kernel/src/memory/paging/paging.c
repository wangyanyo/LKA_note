#include "paging.h"
#include "config.h"
#include "status.h"
#include "memory/heap/kheap.h"

extern void paging_load_directory(uint32_t *directory);

static uint32_t *current_directory = 0;

struct paging_4gb_chunk *paging_new_4gb_chunk(uint8_t flag)
{
        uint32_t *directory = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
        uint32_t offset = 0;
        for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; ++i) {
                uint32_t *entry = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
                for (int j = 0; j < PAGING_TOTAL_ENTRIES_PER_TABLE; ++j) {
                        entry[j] = (offset + j * PAGING_PAGE_SIZE) | flag;
                }
                offset += PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE;
                directory[i] = (uint32_t)entry | flag | PAGING_IS_WRITEABLE;
        }

        struct paging_4gb_chunk *chunk = kzalloc(sizeof(struct paging_4gb_chunk));

        chunk->directory_entry = directory;
        return chunk;
}

void paging_switch(uint32_t *directory)
{
        paging_load_directory(directory);
        current_directory = directory;
}

uint32_t *paging_4gb_chunk_get_directory(struct paging_4gb_chunk *chunk)
{
        return chunk->directory_entry;
}

static int paging_get_indexes(void *virtual, uint32_t *directory_index, uint32_t *table_index)
{
        if (virtual == NULL || directory_index == NULL || table_index == NULL || !paging_is_aligned(virtual)) {
                return -EINVAGS;
        }

        *directory_index = (unsigned long)virtual / (PAGING_PAGE_SIZE * PAGING_TOTAL_ENTRIES_PER_TABLE);
        *table_index = ((unsigned long)virtual % (PAGING_PAGE_SIZE * PAGING_TOTAL_ENTRIES_PER_TABLE)) / PAGING_PAGE_SIZE;
        return KERNEL_ALL_OK;
}

int paging_set(uint32_t *directory, void *virtual, uint32_t val)
{
        if(directory == NULL || !paging_is_aligned(virtual)) {
                return -EINVAGS;
        }

        uint32_t directory_index = 0;
        uint32_t table_index = 0;
        int res = paging_get_indexes(virtual, &directory_index, &table_index);
        if (res != KERNEL_ALL_OK) {
                return res;
        }

        uint32_t directory_entry = directory[directory_index];
        uint32_t *table = (uint32_t *)(directory_entry & 0xFFFFF000);
        table[table_index] = val;

        return KERNEL_ALL_OK;
}

bool paging_is_aligned(void *addr)
{
        return ((unsigned long)addr % PAGING_PAGE_SIZE == 0);        
}

void paging_free_4gb(struct paging_4gb_chunk *chunk)
{
	for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; ++i) {
		uint32_t entry = chunk->directory_entry[i];
		uint32_t *table = (uint32_t *)(entry & 0xFFFFF000);
		kfree(table);
	}

	kfree(chunk->directory_entry);
	kfree(chunk);
}

int paging_map_to(uint32_t *directory, void *virt, void *phys, void *phys_end, int flags)
{
	int res = 0;
	if ((uint32_t)virt % PAGING_PAGE_SIZE) {
		res = -EINVAGS;
		goto out;
	}

	if ((uint32_t)phys % PAGING_PAGE_SIZE) {
		res = -EINVAGS;
		goto out;
	}

	if ((uint32_t)phys_end % PAGING_PAGE_SIZE) {
		res = -EINVAGS;
		goto out;
	}

	if ((uint32_t)phys > (uint32_t)phys_end) {
		res = -EINVAGS;
		goto out;
	}

	uint32_t total_bytes = phys_end - phys;
	int total_pages = total_bytes / PAGING_PAGE_SIZE;
	res = paging_map_range(directory, virt, phys, total_pages, flags);

out:
	return res;
}