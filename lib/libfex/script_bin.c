/*
 * Copyright (C) 2012  Alejandro Mery <amery@geeks.cl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <assert.h>

#include <lib/libfex/common.h>
#include <lib/libfex/script.h>
#include <lib/libfex/script_bin.h>

#define PTR(B, OFF)	(void*)((char*)(B)+(OFF))
#define WORDS(S)	(((S)+(sizeof(uint32_t)-1))/(sizeof(uint32_t)))

/*
 * generator
 */
size_t script_bin_size(struct script *script,
		       size_t *sections, size_t *entries)
{
	size_t words = 0, bin_size = 0;
	struct list_entry *ls, *le;
	struct script_section *section;
	struct script_entry *entry;
	struct script_string_entry *string;

	*sections = *entries = 0;

	/* count */
	for (ls = list_first(&script->sections); ls;
	     ls = list_next(&script->sections, ls)) {
		section = container_of(ls, struct script_section, sections);
		size_t c = 0;

		for (le = list_first(&section->entries); le;
		     le = list_next(&section->entries, le)) {
			size_t size = 0;
			entry = container_of(le, struct script_entry, entries);
			c++;

			switch(entry->type) {
			case SCRIPT_VALUE_TYPE_NULL:
			case SCRIPT_VALUE_TYPE_SINGLE_WORD:
				size = sizeof(uint32_t);
				break;
			case SCRIPT_VALUE_TYPE_STRING:
				string = container_of(entry, struct script_string_entry,
						      entry);
				size = string->l;
				break;
			case SCRIPT_VALUE_TYPE_GPIO:
				size = sizeof(struct script_bin_gpio_value);
				break;
			default:
				putstr("ERROR:Unrecognized value type\n");
			}
			words += WORDS(size);
		}
		if (c>0) {
			*sections += 1;
			*entries += c;
		}
	}

	bin_size = sizeof(struct script_bin_head) +
		(*sections)*sizeof(struct script_bin_section) +
		(*entries)*sizeof(struct script_bin_entry) +
		words*sizeof(uint32_t);

	return bin_size;
}

/*
 * decompiler
 */
static int decompile_section(void *bin,
			     struct script_bin_section *section,
			     struct script *script)
{
	struct script_bin_entry *entry = PTR(bin, section->offset << 2);
	struct script_section *s;
	int i;

	if ((s = script_section_new(script, section->name)) == NULL)
		return 0;

	for (i = section->length; i--; entry++) {
		void *data = PTR(bin, entry->offset << 2);
		unsigned type, words;
		type	= (entry->pattern >> 16) & 0xffff;
		words	= (entry->pattern >>  0) & 0xffff;

		switch(type) {
		case SCRIPT_VALUE_TYPE_SINGLE_WORD: {
			uint32_t *v = data;

			assert(words == 1);

			if (!script_single_entry_new(s, entry->name, *v))
				return 0;

			}; break;
		case SCRIPT_VALUE_TYPE_STRING: {
			size_t bytes = words << 2;
			const char *p, *pe, *v = data;

			for(p=v, pe=v+bytes; *p && p!=pe; p++)
				; /* seek end-of-string */

			if (!script_string_entry_new(s, entry->name, p-v, v))
				return 0;

			}; break;
		case SCRIPT_VALUE_TYPE_GPIO: {
			struct script_bin_gpio_value *gpio = data;
			int32_t v[4];

			assert(words == 6);
			assert(gpio->port > 0 && gpio->port <= GPIO_BANK_MAX);

			v[0] = gpio->mul_sel;
			v[1] = gpio->pull;
			v[2] = gpio->drv_level;
			v[3] = gpio->data;

			if (!script_gpio_entry_new(s, entry->name,
						   gpio->port, gpio->port_num,
						   v))
				return 0;

			}; break;
		case SCRIPT_VALUE_TYPE_NULL:
			if (!script_null_entry_new(s, entry->name))
				return 0;
			break;
		default:
			putstr("ERROR: Unknow section type\n");
		}
	}

	return 1;
}

int script_decompile_bin(void *bin, struct script *script)
{
	int i;
	struct script_bin_head *head = bin;

	putstr("FEX file version: ");
	printhex(head->version[0]);
	putstr(".");
	printhex(head->version[1]);
	putstr(".");
	printhex(head->version[2]);
	putstr("\n");

	putstr("FEX file, parsed 0x");
	printhex(head->sections);
	putstr(" sections\n");

	/* TODO: SANITY: compare head.sections with bin_size */
	for (i=0; i < head->sections; i++) {
		struct script_bin_section *section = &head->section[i];

		if (!decompile_section(bin, section, script))
			return 0;
	}
	return 1;
}
