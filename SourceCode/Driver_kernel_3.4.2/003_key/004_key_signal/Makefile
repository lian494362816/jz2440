
# KERNEL_DIR=/home/black/jz2440/Kernel/linux-2.6.22.6
KERNEL_DIR=/home/black/jz2440/Kernel/linux-3.4.2

PWD=$(shell pwd)
EXTRA_CFLAGS +=-I$(PWD)/include

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules

clean:
	make -C $(KERNEL_DIR) M=$(PWD) modules clean
	rm Module.symvers -f

obj-m += key_signal_drv.o
