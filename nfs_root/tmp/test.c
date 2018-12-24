#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>


int my_open (struct inode *inode, struct file *file)
{
    printk("my open done!\n");
    return 0;
}

ssize_t my_write (struct file *file, const char __user *buf,size_t count, loff_t *ppos)
{
    printk("my write done!\n");
    return 0;
}

static struct file_operations my_fp = {
    .open = my_open,
    .write = my_write,
    .owner = THIS_MODULE,
};

int my_module_init(void)
{
    register_chrdev(100, "module test", &my_fp);
    printk("my module init done\n");
    return 0;
}

void my_module_exit(void)
{
    unregister_chrdev(100, "module test");
    printk("my module exit done\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");

