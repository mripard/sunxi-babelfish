#include <atags.h>
#include <types.h>
#include <version.h>

void main(u32 dummy, u32 machid, const struct tag *tags)
	__attribute__((section(".text_main")));

void main(u32 dummy, u32 machid, const struct tag *tags)
{
	putstr("++ Allwinner Babelfish ");
	putstr(VERSION);
	putstr(" (FEX to DT translator) ++\n\n");

	return 0;
}
