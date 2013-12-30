#ifndef _SOC_H_
#define _SOC_H_

#include <types.h>

#define MACHID_A10		3495
#define COMPATIBLE_A10		"allwinner,sun4i-a10"

#define MACHID_SUN5I		3496
#define COMPATIBLE_A10S		"allwinner,sun5i-a10s"
#define COMPATIBLE_A13		"allwinner,sun5i-a13"

#define MACHID_A20		3497
#define MACHID_A20_ALTERNATE	4283
#define COMPATIBLE_A20		"allwinner,sun7i-a20"

#define MACHID_A31		3892
#define COMPATIBLE_A31		"allwinner,sun6i-a31"

struct soc {
	char	*compatible;
	void	*fdt;
};

struct soc *match_soc(u32 machid);

#endif
