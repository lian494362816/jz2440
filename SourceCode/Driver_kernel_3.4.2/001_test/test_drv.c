#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
//#include <asm/arch/regs-gpio.h>
//#include <asm/hardware.h>

#define MODULE_NAME "module test"
#define MODULE_MAJOR (100)

#define CLASS_DEVICE_MAX (5)

static struct class *test_class;
static struct device *test_class_dev[CLASS_DEVICE_MAX];

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
    int i = 0;
    register_chrdev(MODULE_MAJOR, MODULE_NAME, &my_fp);

    test_class = class_create(THIS_MODULE, "test_class");
    if (IS_ERR(test_class))
    {
        printk("class create failed\n");
        goto err_free_cdev;
    }

    for (i = 0; i < CLASS_DEVICE_MAX; i++)
    {
        test_class_dev[i] = device_create(test_class, NULL, MKDEV(MODULE_MAJOR, i), NULL, "test%d", i);
        if (IS_ERR(test_class_dev[i]))
        {
            printk("class device create failed\n");
            goto err_free_class;
        }
    }
    printk("my module init done\n");
    return 0;

err_free_class:
    for (i -= 1; i >= 0; i--)
    {
        device_destroy(test_class, MKDEV(MODULE_MAJOR, i));
    }
    class_unregister(test_class);
err_free_cdev:
    unregister_chrdev(MODULE_MAJOR, MODULE_NAME);
    return -1;

}

void my_module_exit(void)
{
    int i = 0;

    for (i = 0; i < CLASS_DEVICE_MAX; i++)
    {
        device_destroy(test_class, MKDEV(MODULE_MAJOR, i));
    }
    class_unregister(test_class);

    unregister_chrdev(MODULE_MAJOR, MODULE_NAME);

    printk("my module exit done\n");
}


module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");

