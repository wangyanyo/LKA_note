#ifndef __STATUS_H
#define __STATUS_H

#define KERNEL_ALL_OK 0
#define EIO 1
#define EINVAGS 2
#define ENOMEM 3
#define EBADPATH 4
#define EFSNOTUS 5
#define ERDONLY 6

#define ERROR(value) (void *)(value)
#define ERROR_I(value) (int)(value)
#define IS_ERROR(value) (ERROR_I(value) < 0)

#endif