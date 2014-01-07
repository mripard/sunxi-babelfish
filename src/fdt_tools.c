#include <fdt_tools.h>
#include <print.h>
#include <string.h>

#include <libfdt.h>

/**
 * fdt_alloc_phandle: Return next free phandle value
 *
 * @blob: ptr to device tree
 */
int fdt_alloc_phandle(void *blob)
{
	int offset, phandle = 0, new_phandle;

	for (offset = fdt_next_node(blob, -1, NULL); offset >= 0;
	     offset = fdt_next_node(blob, offset, NULL)) {
		new_phandle = fdt_get_phandle(blob, offset);
		if (new_phandle > phandle)
			phandle = new_phandle;
	}

	return phandle + 1;
}

/*
 * fdt_set_phandle: Create a phandle property for the given node
 *
 * @fdt: ptr to device tree
 * @nodeoffset: node to update
 * @phandle: phandle value to set (must be unique)
 */
int fdt_set_phandle(void *fdt, int nodeoffset, uint32_t phandle)
{
	int ret;

	ret = fdt_setprop_cell(fdt, nodeoffset, "phandle", phandle);
	if (ret < 0)
		return ret;

	/*
	 * For now, also set the deprecated "linux,phandle" property, so that we
	 * don't break older kernels.
	 */
	ret = fdt_setprop_cell(fdt, nodeoffset, "linux,phandle", phandle);

	return ret;
}

/*
 * fdt_create_phandle: Create a phandle property for the given node
 *
 * @fdt: ptr to device tree
 * @nodeoffset: node to update
 */
unsigned int fdt_create_phandle(void *fdt, int nodeoffset)
{
	/* see if there is a phandle already */
	int phandle = fdt_get_phandle(fdt, nodeoffset);

	/* if we got 0, means no phandle so create one */
	if (phandle == 0) {
		int ret;

		phandle = fdt_alloc_phandle(fdt);
		ret = fdt_set_phandle(fdt, nodeoffset, phandle);
		if (ret < 0)
			return 0;
	}

	return phandle;
}

int fdt_add_pinctrl_group(void *fdt, char *pinctrl_path,
			  char *parent_path, struct gpio *gpios,
			  size_t ngpios, char *device)
{
	int parent_offset, pctrl_offset, i, ngroup = 0;

	pctrl_offset = fdt_path_offset(fdt, pinctrl_path);
	if (pctrl_offset < 0)
		return pctrl_offset;

	for (i = 0; i < ngpios; i++) {
		char name[64], pin[8], idx;
		int offset;

		strcpy(name, device);
		strcat(name, "_pins_babelfish");

		switch (gpios[i].pull) {
		case 1:
			strcat(name, "_pullup");
			break;
		case 2:
			strcat(name, "_pulldown");
			break;
		}

		switch (gpios[i].drive) {
		case 1:
			strcat(name, "_20mA");
			break;
		case 2:
			strcat(name, "_30mA");
			break;
		case 3:
			strcat(name, "_40mA");
			break;
		}

		offset = fdt_subnode_offset(fdt, pctrl_offset, name);
		if (offset < 0) {
			char pctrl_prop[16];
			uint32_t phandle;

			/* Create the new pinctrl node */
			offset = fdt_add_subnode(fdt, pctrl_offset, name);
			if (offset < 0)
				return offset;

			/* Setup the properties */
			fdt_setprop_u32(fdt, offset, "allwinner,drive",
					gpios[i].drive);
			fdt_setprop_u32(fdt, offset, "allwinner,pull",
					gpios[i].pull);
			fdt_setprop_string(fdt, offset, "allwinner,function",
					   device);

			/* Register one new default group in the device node */
			parent_offset = fdt_path_offset(fdt, parent_path);
			fdt_appendprop_string(fdt, parent_offset,
					      "pinctrl-names", "default");

			/* Build up the property name for the parent */
			strcpy(pctrl_prop, "pinctrl-");
			idx = '0' + ngroup++;
			strncat(pctrl_prop, &idx, 1);

			/* Create the pinctrl- property in the device node */
			phandle = fdt_get_phandle(fdt, offset);
			fdt_setprop_u32(fdt, parent_offset, pctrl_prop,
					phandle);
		}

		strcpy(pin, "P");
		idx = 'A' + gpios[i].port;
		strncat(pin, &idx, 1);

		idx = '0' + (gpios[i].port_num / 10);
		strncat(pin, &idx, 1);

		idx = '0' + (gpios[i].port_num % 10);
		strncat(pin, &idx, 1);

		fdt_appendprop_string(fdt, offset, "allwinner,pins", pin);
	}

	return 0;
}
