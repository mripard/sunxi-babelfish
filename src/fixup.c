#include <fixup.h>
#include <libfdt.h>
#include <memory.h>
#include <print.h>
#include <string.h>

#define CONSOLE_MAX_LEN		255

#define SUNXI_MAX_UARTS		8

struct gpio {
	uint32_t port;
	uint32_t port_num;
	uint32_t drive;
	uint32_t pull;
};

static int fdt_fixup_add_pinctrl_group(void *fdt, int parent_offset,
				       struct gpio *gpios, size_t ngpios,
				       char *device)
{
	int pctrl_offset, i, ngroup = 0;

	pctrl_offset = fdt_path_offset(fdt, "/soc@01c00000/pinctrl@01c20800");
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
			fdt_appendprop_string(fdt, parent_offset,
					      "pinctrl-names", "default");

			/* Build up the property name for the parent */
			strcpy(pctrl_prop, "pinctrl-");
			idx = '0' + ngroup++;
			strncat(pctrl_prop, &idx, 1);

			/* Create the pinctrl- property in the device node */
			phandle = fdt_get_phandle(fdt, offset);
			fdt_setprop_cell(fdt, parent_offset, pctrl_prop,
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
