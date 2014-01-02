#include <string.h>
#include <types.h>

void *memchr(const void *s, int c, size_t n)
{
	const unsigned char *src = s;
	unsigned char ch = c;

	while ((*src++ != ch) && --n);

	return n ? (void*)src : NULL;
}

int memcmp(const char *s1, const char *s2, size_t n)
{
	while ((*s1++ == *s2++) && --n);
	return (*(unsigned char *)--s1 - *(unsigned char *)--s2);
}

void *memcpy(void *dest, const void *src, size_t n)
{
	const char *in = src;
	char *out = dest;
	int i;

	for (i = 0; i < n; i++)
		out[i] = in[i];

	return dest;
}

void *memmove(void *dest, const void *src, size_t n)
{
	const char *in = src;
	char *out = dest;
	int i;

	if (dest < src) {
		for (i = 0; i < n; i++) {
			out[i] = in[i];
		}
	} else if (dest > src) {
		for (i = (n - 1); i >= 0; i--) {
			out[i] = in[i];
		}
	}

	return dest;
}

void *memset(void *s, int c, size_t n)
{
	char *src = s;
	int i;

	for (i = 0; i < n; i++)
		src[i] = c;

	return s;
}

char *strcat(char *dest, const char *src)
{
	size_t destlen = strlen(dest);
	int i;

	for (i = 0; i < strlen(src); i++)
		dest[destlen + i] = src[i];

	dest[destlen + i] = '\0';

	return dest;
}

char *strchr(const char *s, int c)
{
	while ((*s++ != c) && *s);

	return (*--s == c) ? (void*)s : NULL;
}

int strcmp(const char *s1, const char *s2)
{
	int s1len, s2len;
	int i, res;

	s1len = strlen(s1);
	s2len = strlen(s2);

	if (s1len < s2len)
		return -1;

	if (s1len > s2len)
		return 1;

	for (i = 0; i < s1len; i++) {
		res = s1[i] - s2[i];
		if (res)
			return res;
	}

	return 0;
}

char *strcpy(char *dest, const char *src)
{
	size_t srclen = strlen(src);

	memcpy(dest, src, srclen);

	dest[srclen] = '\0';

	return dest;
}

size_t strlen(const char *s)
{
	size_t i = 0;

	while (*s++)
		i++;

	return i;
}

char *strncat(char *dest, const char *src, size_t n)
{
	size_t destlen = strlen(dest);
	int i;

	for (i = 0; i < strlen(src) && i < n; i++)
		dest[destlen + i] = src[i];

	dest[destlen + i] = '\0';

	return dest;
};
