#include "stdlib.h"
#include "kernel.h"

void *malloc(size_t size)
{
	return kernel_malloc(size);
}

void free(void *ptr)
{

}