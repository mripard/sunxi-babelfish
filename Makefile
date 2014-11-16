# Makefile for Allwinner Babelfish FEX-to-DT translator
AS		:= $(CROSS_COMPILE)as
CC		:= $(CROSS_COMPILE)gcc
OBJCOPY		:= $(CROSS_COMPILE)objcopy
LD		:= $(CROSS_COMPILE)ld
DTC		:= dtc

CFLAGS		:= -Wall -ffreestanding -nostdinc -marm
CFLAGS		+= -I$(CURDIR)/include -I$(CURDIR)/include/generated
CFLAGS		+= -I$(CURDIR)/include/linux -I$(CURDIR)/lib/libfdt

LOADADDR	?= 0x40008000

LDFLAGS		:= -static -nostdlib
LDFLAGS		+= -T babelfish.lds
LDFLAGS		+= -Ttext $(LOADADDR)

OBJS		:=
DTBS		:=

ifndef ZIMAGE
$(error Please provide the path to the zImage)
endif

define my-dir
$(dir $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))
endef

define register-objs
OBJS += $(addprefix $(1), $(2))
endef

include lib/Makefile
include src/Makefile

OBJS := $(addprefix out/, $(OBJS))

include dtsi/Makefile
DTBS += $(addprefix $(FILE_PATH), $(FILE_DTBS))
DTBS := $(addprefix out/, $(DTBS))
DTBS := $(addsuffix .o, $(DTBS))

.PHONY: include/generated/version.h
include/generated/version.h:
	mkdir -p include/generated
	./genver.sh > include/generated/version.h

out/%.o: %.c include/generated/version.h
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -D__KERNEL__ -c -o $@ $<

out/%.o: %.S
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -D__ASSEMBLY__ -c -o $@ $^

out/%.dtb: %.dtsi
	mkdir -p $(dir $@)
	$(DTC) -I dts -O dtb -o $@ $^

out/%.dtb.S: out/%.dtb
	mkdir -p $(dir $@)
	@echo '.section .dtbs.$(*F),"aw"' > $@
	@echo '.global dtb_$(subst -,_,$(*F))' >> $@
	@echo 'dtb_$(subst -,_,$(*F)):' >> $@
	@echo '.incbin "$^"' >> $@

out/zImage.S: $(ZIMAGE)
	@echo '.section .zimage,"a"' > $@
	@echo '.global zImage_start' >> $@
	@echo 'zImage_start:' >> $@
	@echo '.incbin "$^"' >> $@

babelfish: $(OBJS) $(DTBS) out/zImage.o
	$(LD) $(LDFLAGS) -o $@ $^

all: zImage
zImage: babelfish
	$(OBJCOPY) -O binary --set-section-flags .bss=alloc,load,contents $^ $@

uImage: zImage
	mkimage -A arm -O linux -C none -T kernel \
		-a $(LOADADDR) -e $(LOADADDR) \
		-n "Allwinner BabelFish" -d $^ $@

clean:
	rm -fr babelfish zImage include/generated out uImage
