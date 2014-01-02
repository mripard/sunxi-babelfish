#include <atags.h>
#include <fixup.h>
#include <print.h>
#include <soc.h>
#include <types.h>
#include <version.h>

#include <libfdt.h>

#include <lib/libfex/script_bin.h>

#define SCRIPT_BASE_ADDRESS	(void*)0x43000000
#define FDT_BASE_ADDRESS	(void*)0x45000000

extern u32 _binary_zImage_bin_start;

void main(u32 dummy, u32 machid, const struct tag *tags)
	__attribute__((section(".text_main")));

void main(u32 dummy, u32 machid, const struct tag *tags)
{
	void (*start_kernel)(u32 dummy, u32 machid, void *dtb);
	struct script *script;
	struct soc *soc;
	int ret;

	putstr("++ Allwinner Babelfish ");
	putstr(VERSION);
	putstr(" (FEX to DT translator) ++\n\n");

	soc = match_soc(machid);

	putstr("Detected SoC: ");
	if (soc->compatible) {
		putstr(soc->compatible);
		putstr("\n");
	} else {
		putstr("Unknown.\n");
		return;
	}

	script = script_new();
	ret = script_decompile_bin(SCRIPT_BASE_ADDRESS, script);
	if (!ret)
		return;

	ret = fdt_open_into(soc->fdt, FDT_BASE_ADDRESS, 2 * fdt_totalsize(soc->fdt));
	ret = fdt_fixup(soc, FDT_BASE_ADDRESS, script);
	if (ret) {
		putstr("Error in fdt_fixup ");
		printhex(ret);
		putstr("\n");
		return;
	}

	start_kernel = &_binary_zImage_bin_start;
	putstr("Booting Linux...\n");
	start_kernel(0, 0xffffffff, FDT_BASE_ADDRESS);
}
