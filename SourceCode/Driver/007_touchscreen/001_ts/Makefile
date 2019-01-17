
KERNEL_DIR=/home/black/jz2440/Kernel/linux-2.6.22.6

PWD=$(shell pwd)
EXTRA_CFLAGS +=-I$(PWD)/include

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules

clean:
	make -C $(KERNEL_DIR) M=$(PWD) modules clean
	rm Module.symvers -f

obj-m += ts_drv.o
# obj-m += lcd_drv.o
# obj-m += led_platform_device.o
# obj-m += led_platform_driver.o
# obj-m += input_key_drv.o
# obj-m += key_drv.o
# obj-m += key_irq_drv.o
# obj-m += key_lock_atomic_drv.o
# obj-m += key_lock_drv.o
# obj-m += key_lock_mutex_drv.o
# obj-m += key_poll_drv.o
# obj-m += key_signal_drv.o
# obj-m += key_time_drv.o
# obj-m += led_drv.o

