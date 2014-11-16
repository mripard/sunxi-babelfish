#ifndef _SOC_H_
#define _SOC_H_

#include <types.h>

#define SRAM_ID_SUN4I		0x1623
#define COMPATIBLE_A10		"allwinner,sun4i-a10"

#define SRAM_ID_SUN5I		0x1625
#define COMPATIBLE_A10S		"allwinner,sun5i-a10s"
#define COMPATIBLE_A13		"allwinner,sun5i-a13"

#define SRAM_ID_SUN6I		0x1633
#define COMPATIBLE_A31		"allwinner,sun6i-a31"

#define SRAM_ID_SUN7I		0x1651
#define COMPATIBLE_A20		"allwinner,sun7i-a20"

struct soc {
	char	*compatible;
	void	*fdt;
};

struct soc *match_soc();

#endif
