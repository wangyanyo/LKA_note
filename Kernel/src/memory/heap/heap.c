#include "heap.h"
#include "status.h"
#include "memory/memory.h"
#include "terminal/print.h"
#include <stdbool.h>

static int heap_validate_table(void *ptr, void *end, struct heap_table *table)
{
        int res = 0;
        
        size_t table_size = (size_t)(end - ptr);
        size_t total_blocks = table_size / KERNEL_HEAP_BLOCK_SIZE;
        if (table->total != total_blocks) {
                res = -EINVAGS;
                goto out;
        }
out:
        return res;
}

static bool heap_validate_alignment(void *ptr)
{
        return ((unsigned long)ptr % KERNEL_HEAP_BLOCK_SIZE) == 0;
}

int heap_create(struct heap *heap, void *ptr, void *end, struct heap_table *table)
{
        int res = 0;
        if (!heap_validate_alignment(ptr) || !heap_validate_alignment(end)) {
                res = -EINVAGS;
                goto out;
        }

        res = heap_validate_table(ptr, end, table);
        if (res < 0) {
                goto out;
        }
        
        memset(heap, 0x00, sizeof(struct heap));
        heap->table = table;
        heap->saddr = ptr;

        size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total;
        memset(table->enteies, HEAP_BLOCK_TABLE_ENTRY_FREE, table_size);

out:
        return res;
}

static int heap_get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry)
{
        return entry & 0x0F;
}

static int heap_get_start_block(struct heap *heap, uint32_t total_blocks)
{
        struct heap_table *table = heap->table;
        
        int bc = 0;
        int bs = -1;
        for (int i = 0; i < table->total; ++i) {
                if (!heap_get_entry_type(table->enteies[i]) == HEAP_BLOCK_TABLE_ENTRY_FREE) {
                        bc = 0;
                        bs = -1;
                        continue;
                }

                if (bs == -1)
                        bs = i;

                bc++;
                if (bc == total_blocks)
                        break;
        }

        if (bs == -1 || bc != total_blocks) {
                return -ENOMEM;
        }
        return bs;
}

static void *heap_block_to_address(struct heap *heap, uint32_t total_blocks)
{
        return (void *)((char *)heap->saddr + total_blocks * KERNEL_HEAP_BLOCK_SIZE);
}

static void heap_mark_blocks_taken(struct heap *heap, uint32_t start_block, uint32_t total_blocks)
{
        struct heap_table *table = heap->table;
        uint32_t end_block = start_block + total_blocks - 1;
        for (uint32_t i = start_block; i <= end_block; ++i) {
                table->enteies[i] = HEAP_BLOCK_TABLE_ENTRY_TAKEN;
                if (i == start_block)
                        table->enteies[i] |= HEAP_BLOCK_IS_FIRST;
                if (i < end_block)
                        table->enteies[i] |= HEAP_BLOCK_HAS_NEXT;
        }
}

static void *heap_malloc_blocks(struct heap *heap, uint32_t total_blocks)
{
        void *address = 0;
        
        int start_block = heap_get_start_block(heap, total_blocks);
        if (start_block < 0) {
                goto out;
        }

        address = heap_block_to_address(heap, start_block);

        heap_mark_blocks_taken(heap, start_block, total_blocks);

out:
        return address;
}

static size_t heap_align_value_upper(size_t size)
{
        if (size % KERNEL_HEAP_BLOCK_SIZE == 0) {
                return size;
        }

        size -= size % KERNEL_HEAP_BLOCK_SIZE;
        return size + KERNEL_HEAP_BLOCK_SIZE;
}

void *heap_malloc(struct heap *heap, size_t size)
{
        size_t aligned_size = heap_align_value_upper(size);
        uint32_t total_blocks = aligned_size / KERNEL_HEAP_BLOCK_SIZE;
        return heap_malloc_blocks(heap, total_blocks);
}

static void heap_mark_blocks_free(struct heap *heap, uint32_t start_block)
{
        struct heap_table *table = heap->table;
        for (uint32_t i = start_block; i < (uint32_t)table->total; ++i) {
                HEAP_BLOCK_TABLE_ENTRY entry = table->enteies[i];
                table->enteies[i] = HEAP_BLOCK_TABLE_ENTRY_FREE;
                if (!(entry & HEAP_BLOCK_HAS_NEXT))
                        break;

        }
}

static uint32_t heap_address_to_block(struct heap *heap, void *ptr)
{
        return (unsigned long)(ptr - heap->saddr) / KERNEL_HEAP_BLOCK_SIZE;
}

void heap_free(struct heap *heap, void *ptr)
{
        heap_mark_blocks_free(heap, heap_address_to_block(heap, ptr));
}