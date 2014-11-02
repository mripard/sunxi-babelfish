# Makefile for Allwinner Babelfish FEX-to-DT translator
CC		:= $(CROSS_COMPILE)gcc
OBJCOPY		:= $(CROSS_COMPILE)objcopy
LD		:= $(CROSS_COMPILE)ld
DTC		:= dtc

CFLAGS		:= -Wall -ffreestanding -nostdinc
CFLAGS		+= -I$(CURDIR)/include -I$(CURDIR)/include/generated
CFLAGS		+= -I$(CURDIR)/include/linux -I$(CURDIR)/lib/libfdt
CFLAGS		+= -D__KERNEL__

LOADADDR	:= 0x40008000

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

out/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $^

out/%.dtb: %.dtsi
	mkdir -p $(dir $@)
	$(DTC) -I dts -O dtb -o $@ $^
	(cd $(dir $@); \
	$(OBJCOPY) -I binary -O elf32-littlearm -B arm \
		--rename-section .data=.$(notdir $*) $(notdir $@) $(notdir $@);)

version.h:
	mkdir -p include/generated
	./genver.sh > include/generated/version.h

out/zImage.bin: $(ZIMAGE)
	cp $(ZIMAGE) $@
	(cd $(dir $@); \
	$(OBJCOPY) -I binary -O elf32-littlearm -B arm \
		--rename-section .data=.$(notdir $*) $(notdir $@) $(notdir $@);)

babelfish: version.h $(OBJS) $(DTBS) out/zImage.bin
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(DTBS) out/zImage.bin

babelfish.bin: babelfish
	$(OBJCOPY) -O binary --set-section-flags .bss=alloc,load,contents $^ $@

all: uImage
uImage: babelfish.bin
	mkimage -A arm -O linux -C none -T kernel \
		-a $(LOADADDR) -e $(LOADADDR) \
		-n "Allwinner BabelFish" -d $^ $@

clean:
	rm -fr babelfish babelfish.bin include/generated out uImage
