
C_FILES += key.o
S_FILES += start.o
TARGET=test
CROSS_COMPILE=arm-linux-

.PHONY:all clean

all:$(TARGET)
	$(CROSS_COMPILE)ld -Ttext 0 $(S_FILES) $(C_FILES) -o $<.elf
	$(CROSS_COMPILE)objcopy -O binary $<.elf $ $<.bin
	$(CROSS_COMPILE)objdump -D $<.elf > $<.dis
		
$(TARGET):$(C_FILES) $(S_FILES)
	
%.o:%.c
	$(CROSS_COMPILE)gcc -c -o $@ $<

%.o:%.s
	$(CROSS_COMPILE)gcc -c -o $@ $<

clean:
	rm *.bin *.o *.elf *.dis
