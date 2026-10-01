#ifndef __KERNEL_FAT16_H
#define __KERNEL_FAT16_H

#include <stddef.h>

#include "kernel/file.h"

struct filesystem *fat16_init();

#endif