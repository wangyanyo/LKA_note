#ifndef __ISR80H_MISC_H
#define __ISR80H_MISC_H

#include <stddef.h>

struct interrupt_frame;
void *isr80h_command0_sum(struct interrupt_frame *frame);

#endif