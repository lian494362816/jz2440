#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>
#include "my_error.h"


#define LED_MAJOR (0)
#define LED_MODULE_NAME "led module"
#define LED_CLASS_NAME "leds"
#define LED_CLASS_DEV_NAME "led0"

static unsigned int _g_led_major = 0;
static struct class *led_class;
static struct class_dev *led_class_dev;

static volatile unsigned int *gpfcon = NULL;
static volatile unsigned int *gpfdat = NULL;

static int led_dev_open(struct inode *inode, struct file *file)
{
    PRINT_INFO("led open\n");

    /* config to output mode */
    *gpfcon &= ~(0x3 << 8);
    *gpfcon |= (0x1 << 8);

    /* turn off led */
    *gpfdat |= (0x1 << 4);

    return 0;
}

static ssize_t led_dev_write(struct file *file, const char __user *buff, size_t cont, loff_t *ppos)
{
    int val = 0;

    memcpy(&val, buff, sizeof(val));

    if (0 == val)
    {
        *gpfdat |= (0x1 << 4);
        PRINT_INFO("led off");
    }

    if (1 == val)
    {
        *gpfdat &= ~(0x1 << 4);
        PRINT_INFO("led on");
    }

    return 0;
}


static struct file_operations led_dev_fops = {
    .owner = THIS_MODULE,
    .open = led_dev_open,
    .write = led_dev_write,
};

int led_init(void)
{
    _g_led_major = register_chrdev(LED_MAJOR, LED_MODULE_NAME, &led_dev_fops);
    if (_g_led_major < 0)
    {
        PRINT_ERR("register %s failed \n", LED_MODULE_NAME);
        return -1;
    }

    led_class = class_create(THIS_MODULE, LED_CLASS_NAME);
    if (IS_ERR(led_class))
    {
        PRINT_ERR("create %s failed\n", LED_CLASS_NAME);
        goto free_chrdev;
    }

    led_class_dev = class_device_create(led_class, NULL, MKDEV(_g_led_major, 0), NULL, LED_CLASS_DEV_NAME);
    if (IS_ERR(led_class_dev))
    {
        PRINT_ERR("ceate %s failed\n", LED_CLASS_DEV_NAME);
        goto free_class;
    }

    gpfcon = (volatile unsigned int *)ioremap(0x56000050, 8);
    gpfdat = gpfcon + 1;

    PRINT_INFO("%s init ok\n", LED_MODULE_NAME);

    return 0;

free_class:
    class_unregister(led_class);
free_chrdev:
    unregister_chrdev(_g_led_major, LED_MODULE_NAME);
return -1;

}

void led_exit(void)
{
    class_device_unregister(led_class_dev);

    class_unregister(led_class);

    unregister_chrdev(_g_led_major, LED_MODULE_NAME);

    iounmap(gpfcon);

    PRINT_INFO("%s exit \n", LED_MODULE_NAME);
}


module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
