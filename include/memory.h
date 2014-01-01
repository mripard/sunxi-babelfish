#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <types.h>

void *malloc(size_t size);
static inline void free(void *ptr) {};

#endif
