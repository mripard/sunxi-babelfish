FILE_PATH	:= $(call my-dir)

FILE_OBJS	:= head.o
FILE_OBJS	+= fdt_tools.o
FILE_OBJS	+= fixup.o
FILE_OBJS	+= main.o
FILE_OBJS	+= memory.o
FILE_OBJS	+= print.o
FILE_OBJS	+= register.o
FILE_OBJS	+= serial-8250.o
FILE_OBJS	+= string.o
FILE_OBJS	+= soc.o

$(eval $(call register-objs, $(FILE_PATH), $(FILE_OBJS)))
