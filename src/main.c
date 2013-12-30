#include <atags.h>
#include <print.h>
#include <soc.h>
#include <types.h>
#include <version.h>

void main(u32 dummy, u32 machid, const struct tag *tags)
	__attribute__((section(".text_main")));

void main(u32 dummy, u32 machid, const struct tag *tags)
{
	struct soc *soc;

	putstr("++ Allwinner Babelfish ");
	putstr(VERSION);
	putstr(" (FEX to DT translator) ++\n\n");

	soc = match_soc(machid);

	putstr("Detected SoC: ");
	if (soc->compatible)
		putstr(soc->compatible);
	else
		putstr("Unknown.");
	putstr("\n");

	return 0;
}
