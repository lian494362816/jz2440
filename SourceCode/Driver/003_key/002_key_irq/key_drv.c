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

#define KEY_MAJOR (0)
#define KEY_MODULE_NAME "key module"
#define KEY_CLASS_NAME "key_class"
#define KEY_CLASS_DEV_NAME "keys"

static struct class *key_class = NULL;
static struct class_dev  *key_class_dev = NULL;
static unsigned int key_major = 0;

static volatile unsigned int *gpfcon = NULL;
static volatile unsigned int *gpfdat = NULL;

static volatile unsigned int *gpgcon = NULL;
static volatile unsigned int *gpgdat = NULL;

int key_open(struct inode *inode, struct file *file)
{
    /* set gpf0,2 gpg3,11 to input mode */
    *gpfcon &= ~(0x3 << (0 * 2) | 0x3 << (2 *2 ));
    *gpgcon &= ~(0x3 << (3 * 2) | 0x3 << (11 * 2));

    PRINT_INFO("open ok\n");
    return 0;
}


ssize_t key_read(struct file *file, char __user *userbuf, size_t count, loff_t *ppos)
{
    int key_val[4] = {0};

    if (count < sizeof(key_val))
    {
        PRINT_ERR("min size is %d \n", sizeof(key_val));
        return -1;
    }

    key_val[0] = (*gpfdat & (0x1 << 0))  ? 1 : 0;
    key_val[1] = (*gpfdat & (0x1 << 2))  ? 1 : 0;
    key_val[2] = (*gpgdat & (0x1 << 3))  ? 1 : 0;
    key_val[3] = (*gpgdat & (0x1 << 11)) ? 1 : 0;

    copy_to_user(userbuf, key_val, sizeof(key_val));

    return sizeof(key_val);
}


static struct file_operations key_fops = {
    .owner = THIS_MODULE,
    .open = key_open,
    .read = key_read,
};

int key_init(void)
{
    key_major = register_chrdev(KEY_MAJOR, KEY_MODULE_NAME, &key_fops);
    if (key_major < 0)
    {
        PRINT_ERR("register %s fail\n", KEY_MODULE_NAME);
        return -1;
    }

    /* create /sys/class/key_class */
    key_class = class_create(THIS_MODULE, KEY_CLASS_NAME);
    if (IS_ERR(key_class))
    {
        PRINT_ERR("create %s fail\n", KEY_CLASS_NAME);
        goto free_chrdev;
    }

    /* create /dev/keys */
    key_class_dev = class_device_create(key_class, NULL, MKDEV(key_major, 0), NULL, KEY_CLASS_DEV_NAME);
    if (IS_ERR(key_class_dev))
    {
        PRINT_ERR("create %s fail\n", KEY_CLASS_DEV_NAME);
        goto free_class;
    }

    gpfcon = (volatile unsigned int *)ioremap(0x56000050, 16);
    gpfdat = gpfcon + 1;
    gpgcon = (volatile unsigned int *)ioremap(0x56000060, 16);
    gpgdat = gpgcon + 1;

    PRINT_INFO("%s init\n", KEY_MODULE_NAME);

    return 0;

free_class:
    class_unregister(key_class);
free_chrdev:
    unregister_chrdev(key_major, KEY_MODULE_NAME);
    return -1;
}

void key_exit(void)
{
    class_device_unregister(key_class_dev);
    class_unregister(key_class);

    unregister_chrdev(key_major, KEY_MODULE_NAME);

    iounmap(gpfcon);
    iounmap(gpgcon);

    PRINT_INFO("%s exit\n", KEY_MODULE_NAME);
}

module_init(key_init);
module_exit(key_exit);

MODULE_LICENSE("GPL");
