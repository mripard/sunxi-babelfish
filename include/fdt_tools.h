#ifndef _FDT_TOOLS_H_
#define _FDT_TOOLS_H_

#include <linux/types.h>

int fdt_alloc_phandle(void *blob);
int fdt_set_phandle(void *fdt, int nodeoffset, uint32_t phandle);
unsigned int fdt_create_phandle(void *fdt, int nodeoffset);
#endif
