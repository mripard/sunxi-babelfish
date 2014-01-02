#include <fixup.h>
#include <libfdt.h>
#include <memory.h>
#include <print.h>
#include <string.h>

#define CONSOLE_MAX_LEN		255

#define SUNXI_MAX_UARTS		8

static int fdt_fixup_bootargs(struct soc *soc, void *fdt, struct script *script)
{
	struct script_single_entry *entry;
	struct script_section *section;
	char *bootargs, idx;
	int offset, ret;

	bootargs = malloc(CONSOLE_MAX_LEN);
	strcpy(bootargs, "earlyprintk ");

	/* Set default UART to output on */
	section = script_find_section(script, "uart_para");
	entry = (struct script_single_entry *)script_find_entry(section, "uart_debug_port");
	idx = '0' + entry->value;

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

static int fdt_fixup_machine(struct soc *soc, void *fdt, struct script *script)
{
	struct script_string_entry *entry;
	struct script_section *section;
	int ret;

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

	return 0;
}

static int fdt_fixup_uarts(struct soc *soc, void *fdt, struct script *script)
{
	struct script_single_entry *entry;
	struct script_section *section;
	char *fex_name, *alias, idx;
	int offset, ret, i;

	fex_name = malloc(strlen("uart_para") + 2);
	alias = malloc(strlen("serial") + 2);
	for (i = 0; i < SUNXI_MAX_UARTS; i++) {
		idx = '0' + i;

		strcpy(fex_name, "uart_para");
		strncat(fex_name, &idx, 1);

		section = script_find_section(script, fex_name);
		entry = (struct script_single_entry *)script_find_entry(section, "uart_used");

		if (entry->value == 1) {
			strcpy(alias, "serial");
			strncat(alias, &idx, 1);

			offset = fdt_path_offset(fdt, alias);
			ret = fdt_setprop_string(fdt, offset, "status", "okay");
			if (ret)
				return ret;
		}
	}

	return 0;
}

int fdt_fixup(struct soc *soc, void *fdt, struct script *script)
{
	int ret;

	ret = fdt_fixup_machine(soc, fdt, script);
	if (ret)
		return ret;

	ret = fdt_fixup_bootargs(soc, fdt, script);
	if (ret)
		return ret;

	ret = fdt_fixup_uarts(soc, fdt, script);
	if (ret)
		return ret;

	return 0;
}
