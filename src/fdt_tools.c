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
