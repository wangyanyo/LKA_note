#ifndef __ISR80H_HEAP_H
#define __ISR80H_HEAP_H

#include <stddef.h>

struct interrupt_frame;
void *isr80h_command4_malloc(struct interrupt_frame *frame);
void *isr80h_command5_free(struct interrupt_frame *frame);

#endif