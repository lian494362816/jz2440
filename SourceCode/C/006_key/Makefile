
all:
	arm-linux-gcc -c -o key.o key.c
	arm-linux-gcc -c -o start.o start.s
	arm-linux-ld -Ttext 0  start.o key.o -o key.elf
	arm-linux-objcopy -O binary key.elf key.bin
	arm-linux-objdump -D key.elf > key.dis

clean:
	rm *.bin *.o *.elf *.dis
