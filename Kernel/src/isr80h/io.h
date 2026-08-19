#ifndef __ISR80H_IO_H
#define __ISR80H_IO_H

struct interrupt_frame;
void *isr80h_command0_print(struct interrupt_frame *frame);

#endif