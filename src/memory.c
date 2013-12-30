#include <types.h>

void *malloc_base = 0x44000000;

/* Dummy memory allocator */
void *malloc(size_t size)
{
	void *ret = malloc_base;
	malloc_base += size;

	return ret;
}
