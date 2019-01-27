
TOP_DIR:=$(shell pwd)
TARGET:=$(TOP_DIR)/test
LIB_PATH:=$(TOP_DIR)/lib
INCLUDE_PATH:=$(TOP_DIR)/include
LINK_FILE:=$(TOP_DIR)/test.lds
OBJS:=$(shell find . -type f)
OBJS:=$(filter %.c %.s %S, $(OBJS))
OBJS:=$(patsubst %.c, %.o, $(OBJS))
OBJS:=$(patsubst %.s, %.o, $(OBJS))
OBJS:=$(patsubst %.S, %.o, $(OBJS))

CROSS_COMPILE=arm-linux-
CFLAGS = -Werror -I$(INCLUDE_PATH) -march=armv4 -g  -Os   -fno-strict-aliasing   -nostdlib -fno-common -ffixed-r8 -msoft-float -malignment-traps
# CFLAGS = -Werror -nostdinc -nostdlib  -march=armv4 -fno-builtin

RM= rm -f

all: $(OBJS) build 

build:
	$(CROSS_COMPILE)ld  -T $(LINK_FILE) $(OBJS)  -o $(TARGET).elf
	$(CROSS_COMPILE)objcopy -O binary $(TARGET).elf $(TARGET).bin
	$(CROSS_COMPILE)objdump -D $(TARGET).elf > $(TARGET).dis

%.o:%.c
	$(CROSS_COMPILE)gcc $(CFLAGS) -c -o $@ $< -MD -MF .$@.d

%.o:%.s
	$(CROSS_COMPILE)gcc $(CFLAGS) -c -o $@ $<

%.o:%.S
	$(CROSS_COMPILE)gcc $(CFLAGS) -c -o $@ $<



clean:
	$(RM) $(TARGET).elf  $(TARGET).bin $(TARGET).dis $(RM) $(OBJS)

.PHONY:  all clean build
