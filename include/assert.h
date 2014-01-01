#ifndef _ASSERT_H_
#define _ASSERT_H_

#include <print.h>

#define STRINGIFY(x)	#x

#define assert(expr)						\
	do {							\
		if (!expr) {					\
			putstr("Assertion failed! ");		\
			putstr(#expr);				\
			putstr(", ");				\
			putstr(__FILE__);			\
			putstr(", ");				\
			putstr(__func__);			\
			putstr(", line ");			\
			putstr(STRINGIFY(__LINE__));		\
			putstr("\n");				\
		}						\
	} while (0);
#endif
