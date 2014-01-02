#include <fixup.h>
#include <libfdt.h>
#include <memory.h>
#include <print.h>
#include <string.h>

#define CONSOLE_MAX_LEN		255

int fdt_fixup(struct soc *soc, void *fdt, struct script *script)
{
	struct script_section *section;
	struct script_string_entry *str_entry;
	struct script_single_entry *sword_entry;
	char *bootargs;
	int ret, offset;
	char idx;

	bootargs = malloc(CONSOLE_MAX_LEN);
	strcpy(bootargs, "earlyprintk ");

	/* Set machine compatible (offset 0 is the root node) */
	ret = fdt_setprop_string(fdt, 0, "compatible", soc->compatible);
	if (ret < 0)
		return ret;

	/* Set machine model (offset 0 is the root node) */
	section = script_find_section(script, "product");
	str_entry = (struct script_string_entry *)script_find_entry(section, "machine");
	ret = fdt_setprop_string(fdt, 0, "model", str_entry->string);
	if (ret < 0)
		return ret;

	/* Set default UART to output on */
	section = script_find_section(script, "uart_para");
	sword_entry = (struct script_single_entry *)script_find_entry(section, "uart_debug_port");
	idx = '0' + sword_entry->value;

	strcat(bootargs, "console=ttyS");
	strncat(bootargs, &idx, 1);
	strcat(bootargs, ",115200 ");

	/* Set bootargs */
	offset = fdt_path_offset(fdt, "/chosen");
	if (offset < 0) {
		offset = fdt_add_subnode(fdt, 0, "chosen");

		if (offset < 0)
			return offset;
	}

	ret = fdt_setprop_string(fdt, offset, "bootargs", bootargs);
	if (ret < 0)
		return ret;

	return 0;
}
