# Makefile for Allwinner Babelfish FEX-to-DT translator

LOADADDR	:= 0x40008000

CC		:= $(CROSS_COMPILE)gcc
OBJCOPY		:= $(CROSS_COMPILE)objcopy
LD		:= $(CROSS_COMPILE)ld
DTC		:= dtc

CFLAGS		:= -Wall -ffreestanding
CFLAGS		+= -I./include -I./include/generated

LDFLAGS		:= -static -nostdlib
LDFLAGS		+= -T babelfish.lds
LDFLAGS		+= -Ttext $(LOADADDR)

COMMON_OBJS 	:= \
			main.o \
			print.o \
			register.o \
			serial-8250.o

all: uImage

version.h:
	mkdir -p include/generated
	./genver.sh > include/generated/version.h

out/%.o: src/%.c
	mkdir -p out
	$(CC) $(CFLAGS) -c -o $@ $^

babelfish: version.h $(addprefix out/, $(COMMON_OBJS))
	$(LD) $(LDFLAGS) -o $@ $(addprefix out/, $(COMMON_OBJS))

babelfish.bin: babelfish
	$(OBJCOPY) -O binary --set-section-flags .bss=alloc,load,contents $^ $@

uImage: babelfish.bin
	mkimage -A arm -O linux -C none -T kernel \
		-a $(LOADADDR) -e $(LOADADDR) \
		-n "Allwinner BabelFish" -d $^ $@

clean:
	rm -fr babelfish babelfish.bin include/generated out uImage
