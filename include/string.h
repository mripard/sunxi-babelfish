#ifndef _STRING_H_
#define _STRING_H_

#include <types.h>

void *memchr(const void *s, int c, size_t n);
int memcmp(const char *s1, const char *s2, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);

char *strchr(const char *s, int c);
int strcmp(const char *s1, const char *s2);
size_t strlen(const char *s);

#endif
