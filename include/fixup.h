#ifndef _FIXUP_H_
#define _FIXUP_H_

#include <lib/libfex/script.h>
#include <soc.h>

#include <uapi/asm/setup.h>

int fdt_fixup(struct soc *soc, void *fdt, struct script *script, const struct tag *atags);

#endif
