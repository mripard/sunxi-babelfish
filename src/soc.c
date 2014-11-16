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

#define SRAM_BASE	0x01c00000

struct soc *match_soc()
{
	struct soc *soc;
	char *cpt;
	void *fdt;
	u32 reg;

	reg = readl(SRAM_BASE + 0x24);
	writel(reg | (1 << 15), SRAM_BASE + 0x24);

	reg = readl(SRAM_BASE + 0x24);
	reg = (reg >> 16) & 0xffff;

	switch (reg) {
	case SRAM_ID_SUN4I:
		fdt = &dtb_sun4i_a10;
		cpt = COMPATIBLE_A10;
		break;

	/*
	 * A10s and A13 seem to share the same SRAM ID. We need to be
	 * smarter than that and lookup in the SID.
	 */
	case SRAM_ID_SUN5I:
		reg = readl(SID_BASE + 0x08);

		switch ((reg >> 12) & 0xf) {
		case 3:
			fdt = &dtb_sun5i_a13;
			cpt = COMPATIBLE_A13;
			break;

		case 7:
			fdt = &dtb_sun5i_a10s;
			cpt = COMPATIBLE_A10S;
			break;
		}

		break;

	case SRAM_ID_SUN6I:
		fdt = &dtb_sun6i_a31;
		cpt = COMPATIBLE_A31;
		break;

	case SRAM_ID_SUN7I:
		fdt = &dtb_sun7i_a20;
		cpt = COMPATIBLE_A20;
		break;

	default:
		putstr("Unknown sram-id: 0x");
		printhex(reg);
		putstr("\n");
		return NULL;
	}

	soc = malloc(sizeof(struct soc));
	if (!soc)
		return NULL;

	soc->fdt = fdt;
	soc->compatible = cpt;

	return soc;
}
