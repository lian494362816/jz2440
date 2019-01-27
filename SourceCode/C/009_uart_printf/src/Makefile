#S_FILES += start.o 
 S_FILES += start.o lib1funcs.o
C_FILES:=$(wildcard *.c) 
OBJS:= $(patsubst %.c, %.o, $(C_FILES))
DEP_FILES:= $(patsubst %, .%.d, $(OBJS))
DEP_FILES:= $(wildcard $(DEP_FILES))
TARGET=test
CROSS_COMPILE=arm-linux-
CFLAGS = -Werror -I../include


all:$(TARGET)
		
$(TARGET):$(S_FILES)  $(OBJS) 
	 $(CROSS_COMPILE)ld -Ttext 0 -Tdata 0xf4c $^ -o $@.elf
	 $(CROSS_COMPILE)objcopy -O binary $@.elf $ $@.bin
	 $(CROSS_COMPILE)objdump -D $@.elf > $@.dis
	
ifneq ($(DEP_FILES), )
include $(DEP_FILES)
endif
	
%.o:%.c
	$(CROSS_COMPILE)gcc $(CFLAGS) -c -o $@ $< -MD -MF .$@.d

%.o:%.s
	$(CROSS_COMPILE)gcc $(CFLAGS) -c -o $@ $<

%.o:%.S
	$(CROSS_COMPILE)gcc $(CFLAGS) -c -o $@ $<

clean:
	rm *.bin *.o *.elf *.dis $(DEP_FILES) 

.PHONY: clean all
