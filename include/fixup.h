#ifndef _FIXUP_H_
#define _FIXUP_H_

#include <script.h>
#include <soc.h>

int fdt_fixup(struct soc *soc, void *fdt, struct script *script);

#endif
