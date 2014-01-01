#include <string.h>
#include <types.h>

void *memcpy(void *dest, const void *src, size_t n)
{
	const char *in = src;
	char *out = dest;
	int i;

	for (i = 0; i < n; i++)
		out[i] = in[i];

	return dest;
}

int strcmp(const char *s1, const char *s2)
{
	while ((*s1++ == *s2++) && *s1);
	return (*(unsigned char *)--s1 - *(unsigned char *)--s2);
}

size_t strlen(const char *s)
{
	size_t i = 0;

	while (*s++)
		i++;

	return i;
}
