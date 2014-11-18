#include <fdt_tools.h>
#include <fixup.h>
#include <libfdt.h>
#include <memory.h>
#include <print.h>
#include <string.h>

#include <uapi/asm/setup.h>

#define FDT_PINCTRL_PATH	"/soc@01c00000/pinctrl@01c20800"
#define FIXUP_BASE_CMDLINE	"earlyprintk"
#define CONSOLE_MAX_LEN		255
#define SUNXI_MAX_UARTS		8

static int fdt_fixup_append_bootargs(void *fdt, const char *bootargs)
{
	int offset, ret, len;
	const char *old_args = NULL;
	char *new_args;

	offset = fdt_path_offset(fdt, "/chosen");
	if (offset < 0) {
		offset = fdt_add_subnode(fdt, 0, "chosen");

		if (offset < 0)
			return offset;
	}

	old_args = fdt_getprop(fdt, offset, "bootargs", &len);

	if (len > 0) {
		new_args = malloc(len + strlen(bootargs) + 2);
		strcpy(new_args, old_args);
		strcat(new_args, " ");
		strcat(new_args, bootargs);
	} else {
		new_args = malloc(strlen(bootargs) + 2);
		strcpy(new_args, bootargs);
	}

	ret = fdt_setprop_string(fdt, offset, "bootargs", new_args);
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
	int offset, ret, i;
	char bootargs[64];
	char idx;

	/* Set default UART to output on */
	section = script_find_section(script, "uart_para");
	entry = (struct script_single_entry *)script_find_entry(section, "uart_debug_port");
	idx = '0' + entry->value;

	strcpy(bootargs, "console=ttyS");
	strncat(bootargs, &idx, 1);
	strcat(bootargs, ",115200");

	fdt_fixup_append_bootargs(fdt, bootargs);

	for (i = 0; i < SUNXI_MAX_UARTS; i++) {
		struct script_gpio_entry *gpio_entry;
		char fex_name[16], dt_name[8], alias[8];
		struct gpio *gpios;

		idx = '0' + i;

		strcpy(fex_name, "uart_para");
		strncat(fex_name, &idx, 1);

		section = script_find_section(script, fex_name);
		entry = (struct script_single_entry *)script_find_entry(section, "uart_used");

		if (entry->value != 1)
			continue;

		strcpy(alias, "serial");
		strncat(alias, &idx, 1);

		offset = fdt_path_offset(fdt, alias);
		ret = fdt_setprop_string(fdt, offset, "status", "okay");
		if (ret)
			return ret;

		entry = (struct script_single_entry *)
			script_find_entry(section, "uart_type");

		gpios = malloc(entry->value * sizeof(*gpios));

		switch (entry->value) {
		case 8:
			gpio_entry = (struct script_gpio_entry *)
				script_find_entry(section, "uart_dtr");
			GPIO_ENTRY_TO_STRUCT(gpios, 7, gpio_entry);

			gpio_entry = (struct script_gpio_entry *)
				script_find_entry(section, "uart_dsr");
			GPIO_ENTRY_TO_STRUCT(gpios, 6, gpio_entry);

			gpio_entry = (struct script_gpio_entry *)
				script_find_entry(section, "uart_dcd");
			GPIO_ENTRY_TO_STRUCT(gpios, 5, gpio_entry);

			gpio_entry = (struct script_gpio_entry *)
				script_find_entry(section, "uart_ring");
			GPIO_ENTRY_TO_STRUCT(gpios, 4, gpio_entry);
		case 4:
			gpio_entry = (struct script_gpio_entry *)
				script_find_entry(section, "uart_cts");
			GPIO_ENTRY_TO_STRUCT(gpios, 3, gpio_entry);

			gpio_entry = (struct script_gpio_entry *)
				script_find_entry(section, "uart_rts");
			GPIO_ENTRY_TO_STRUCT(gpios, 2, gpio_entry);
		case 2:
			gpio_entry = (struct script_gpio_entry *)
				script_find_entry(section, "uart_rx");
			GPIO_ENTRY_TO_STRUCT(gpios, 1, gpio_entry);

			gpio_entry = (struct script_gpio_entry *)
				script_find_entry(section, "uart_tx");
			GPIO_ENTRY_TO_STRUCT(gpios, 0, gpio_entry);
			break;
		default:
			continue;
		}

		strcpy(dt_name, "uart");
		strncat(dt_name, &idx, 1);

		fdt_add_pinctrl_group(fdt, FDT_PINCTRL_PATH, alias, gpios, entry->value,
				      dt_name);
	}

	return 0;
}

int fdt_fixup_from_atags(void *fdt, const struct tag *atags)
{
	const struct tag *atag = atags;
	uint32_t mem_reg[2];
	int offset;

	for_each_tag(atag, atags) {
		if (atag->hdr.tag == ATAG_CMDLINE) {
			fdt_fixup_append_bootargs(fdt, atag->u.cmdline.cmdline);
		} else if (atag->hdr.tag == ATAG_MEM) {

			if (!atag->u.mem.size)
				continue;

			mem_reg[0] = cpu_to_fdt32(atag->u.mem.start);
			mem_reg[1] = cpu_to_fdt32(atag->u.mem.size);

			offset = fdt_path_offset(fdt, "/memory");
			fdt_setprop(fdt, offset, "reg", mem_reg, sizeof(mem_reg));
		} else if (atag->hdr.tag == ATAG_INITRD2) {
			uint32_t initrd_start, initrd_size;

			initrd_start = atag->u.initrd.start;
			initrd_size = atag->u.initrd.size;

			offset = fdt_path_offset(fdt, "/chosen");
			fdt_setprop_cell(fdt, offset, "linux,initrd-start",
					 initrd_start);
			fdt_setprop_cell(fdt, offset, "linux,initrd-end",
					 initrd_start + initrd_size);
		}
	}

	return 0;
}

int fdt_fixup_from_fex(struct soc *soc, void *fdt, struct script *script)
{
	int ret;

	ret = fdt_fixup_append_bootargs(fdt, FIXUP_BASE_CMDLINE);
	if (ret)
		return ret;

	ret = fdt_fixup_machine(soc, fdt, script);
	if (ret)
		return ret;

	ret = fdt_fixup_uarts(soc, fdt, script);
	if (ret)
		return ret;

	return 0;
}
