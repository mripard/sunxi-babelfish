#include <fixup.h>
#include <libfdt.h>
#include <print.h>

int fdt_fixup(struct soc *soc, void *fdt, struct script *script)
{
	struct script_section *section;
	struct script_string_entry *entry;
	int ret, offset;

	/* Set machine compatible (offset 0 is the root node) */
	ret = fdt_setprop_string(fdt, 0, "compatible", soc->compatible);
	if (ret < 0)
		return ret;

	/* Set machine model (offset 0 is the root node) */
	section = script_find_section(script, "product");
	entry = (struct script_string_entry *)script_find_entry(section, "machine");
	ret = fdt_setprop_string(fdt, 0, "model", entry->string);
	if (ret < 0)
		return ret;

	/* Set bootargs */
	offset = fdt_path_offset(fdt, "/chosen");
	if (offset < 0) {
		offset = fdt_add_subnode(fdt, 0, "chosen");

		if (offset < 0)
			return offset;
	}

	ret = fdt_setprop_string(fdt, offset, "bootargs", "earlyprintk");
	if (ret < 0)
		return ret;

	return 0;
}
