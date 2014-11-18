#ifndef _FIXUP_H_
#define _FIXUP_H_

#include <lib/libfex/script.h>
#include <soc.h>

#include <uapi/asm/setup.h>

int fdt_fixup_from_atags(void *fdt, const struct tag *atags);
int fdt_fixup_from_fex(struct soc *soc, void *fdt, struct script *script);

#endif
