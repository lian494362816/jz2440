KERNEL_DIR=/home/black/jz2440/Kernel/linux-2.6.22.6

all:
	make -C $(KERNEL_DIR) M=`pwd` modules

clean:
	make -C $(KERNEL_DIR) M=`pwd` modules clean
	rm Module.symvers -f

obj-m += test_drv.o
