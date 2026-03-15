#include "kheap.h"
#include "heap.h"
#include "config.h"
#include "terminal/print.h"
#include "memory/memory.h"

struct heap kernel_heap;
struct heap_table kernel_heap_table;

void kheap_init()
{
        kernel_heap_table.enteies = (HEAP_BLOCK_TABLE_ENTRY *)KERNEL_HEAP_TABLE_ADDRESS;
        kernel_heap_table.total = KERNEL_HEAP_SIZE_BYTES / KERNEL_HEAP_BLOCK_SIZE;

        void *end = (void *)(KERNEL_HEAP_ADDRESS + KERNEL_HEAP_SIZE_BYTES);
        int res = heap_create(&kernel_heap, (void *)(KERNEL_HEAP_ADDRESS), end, &kernel_heap_table);
        if(res < 0) {
                terminal_print("Failed to create heap!\n");
        }
}

void *kmalloc(size_t size)
{
        return heap_malloc(&kernel_heap, size);
}

void *kzalloc(size_t size)
{
        void *ptr = heap_malloc(&kernel_heap, size);
        if (!ptr)
                return 0;

        memset(ptr, 0x00, size);
        return ptr;
}

void kfree(void *ptr)
{
        heap_free(&kernel_heap, ptr);
}