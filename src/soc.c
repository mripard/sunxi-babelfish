#include <memory.h>
#include <print.h>
#include <register.h>
#include <soc.h>
#include <types.h>

extern u32 dtb_sun4i_a10;
extern u32 dtb_sun5i_a10s;
extern u32 dtb_sun5i_a13;
extern u32 dtb_sun6i_a31;
extern u32 dtb_sun7i_a20;

struct soc *match_soc(u32 machid)
{
	struct soc *soc = malloc(sizeof(struct soc));
	u32 sid_detect;

	switch (machid) {
	case MACHID_A10:
		soc->fdt = &dtb_sun4i_a10;
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
			soc->fdt = &dtb_sun5i_a13;
			soc->compatible = COMPATIBLE_A13;
			break;

		case 7:
			soc->fdt = &dtb_sun5i_a10s;
			soc->compatible = COMPATIBLE_A10S;
			break;
		}
		break;

	case MACHID_A20:
	case MACHID_A20_ALTERNATE:
		soc->fdt = &dtb_sun7i_a20;
		soc->compatible = COMPATIBLE_A20;
		break;

	case MACHID_A31:
		soc->fdt = &dtb_sun6i_a31;
		soc->compatible = COMPATIBLE_A31;
		break;

	default:
		putstr("Unrecognized mach-id: 0x");
		printhex(machid);
		putstr("\n");
		return NULL;
	}

	return soc;
}
