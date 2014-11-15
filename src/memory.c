#include <types.h>

extern void *heap_base;

/* Dummy memory allocator */
void *malloc(size_t size)
{
	void *ret = heap_base;
	size_t aligned;

	aligned = (((size - 1) / 4) + 1) * 4;

	heap_base += aligned;

	return ret;
}
