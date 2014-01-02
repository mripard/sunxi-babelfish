#include <memory.h>
#include <print.h>
#include <register.h>
#include <soc.h>
#include <types.h>

extern u32 _binary_sun4i_a10_dtb_start;
extern u32 _binary_sun5i_a10s_dtb_start;
extern u32 _binary_sun5i_a13_dtb_start;
extern u32 _binary_sun6i_a31_dtb_start;
extern u32 _binary_sun7i_a20_dtb_start;

struct soc *match_soc(u32 machid)
{
	struct soc *soc = malloc(sizeof(struct soc));
	u32 sid_detect;

	switch (machid) {
	case MACHID_A10:
		soc->fdt = &_binary_sun4i_a10_dtb_start;
		soc->compatible = COMPATIBLE_A10;
		break;

	/* 
	 * A10s and A13 seem to share the same mach-id. We need to be
	 * smarter than that and lookup in the SID.
	 */
	case MACHID_SUN5I:
		sid_detect = readl(SID_BASE + 0x08);

		switch ((sid_detect >> 12) & 0xf) {
		case 3:
			soc->fdt = &_binary_sun5i_a13_dtb_start;
			soc->compatible = COMPATIBLE_A13;
			break;

		case 7:
			soc->fdt = &_binary_sun5i_a10s_dtb_start;
			soc->compatible = COMPATIBLE_A10S;
			break;
		}
		break;

	case MACHID_A20:
	case MACHID_A20_ALTERNATE:
		soc->fdt = &_binary_sun7i_a20_dtb_start;
		soc->compatible = COMPATIBLE_A20;
		break;

	case MACHID_A31:
		soc->fdt = &_binary_sun6i_a31_dtb_start;
		soc->compatible = COMPATIBLE_A31;
		break;

	default:
		putstr("Unrecognized mach-id: 0x");
		printhex(machid);
		putstr("\n");
	}

	return soc;
}
