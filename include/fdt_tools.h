#ifndef _FDT_TOOLS_H_
#define _FDT_TOOLS_H_

#include <linux/types.h>

#define GPIO_ENTRY_TO_STRUCT(array, idx, entry)				\
	do {								\
		array[idx].port = entry->port;				\
		array[idx].port_num = entry->port_num;			\
		array[idx].drive = entry->data[1];			\
		array[idx].pull = entry->data[2];			\
	} while (0)

struct gpio {
	uint32_t port;
	uint32_t port_num;
	uint32_t drive;
	uint32_t pull;
};

int fdt_alloc_phandle(void *blob);
int fdt_set_phandle(void *fdt, int nodeoffset, uint32_t phandle);
unsigned int fdt_create_phandle(void *fdt, int nodeoffset);

int fdt_add_pinctrl_group(void *fdt, char *pinctrl_path,
			  char *parent_path, struct gpio *gpios,
			  size_t ngpios, char *device);

#endif
