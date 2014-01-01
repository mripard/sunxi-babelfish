#include <types.h>

void *malloc_base = (void *)0x44000000;

/* Dummy memory allocator */
void *malloc(size_t size)
{
	void *ret = malloc_base;
	size_t aligned;

	aligned = (((size - 1) / 4) + 1) * 4;

	malloc_base += aligned;

	return ret;
}
