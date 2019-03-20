#include <linux/device.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include <linux/types.h>
#include <linux/cdev.h>
#include "my_error.h"


#define CHR_MODULE_NAME "new_chr"
#define CHR_MOUDLE_CLASS_NAME "new_chr_class"
#define CHR_MODULE_MAX (2)
static unsigned int chr_major = 0;
static struct cdev chr_cdev;
static struct cdev chr_cdev2;

static struct class *chr_class = NULL;

static int chr_open(struct inode *inode, struct file *file)
{
    PRINT_INFO("open new chr ok \n");

    return 0;
}

static int chr_open2(struct inode *inode, struct file *file)
{
    PRINT_INFO("open 2 new chr ok \n");

    return 0;
}


struct file_operations chr_fops = {
    .owner = THIS_MODULE,
    .open = chr_open,
};

struct file_operations chr_fops2 = {
    .owner = THIS_MODULE,
    .open = chr_open2,
};

static __init int new_chr_init(void)
{
    dev_t dev;
    int ret = 0;
    int i = 0;

    /* minor 0 ~ minor CHR_MODULE_MAX - 1 --> chr_fops*/
    if (chr_major)
    {
        dev = MKDEV(chr_major, 0);
        ret = register_chrdev_region(dev, CHR_MODULE_MAX, CHR_MODULE_NAME);
        if (ret)
        {
            PRINT_ERR("reg chrdev region fail \n");
            return -1;
        }
    }
    else
    {
        ret = alloc_chrdev_region(&dev, 0, CHR_MODULE_MAX, CHR_MODULE_NAME);
        if (ret)
        {
            PRINT_ERR("alloc chrdev region fail \n");
            return -1;
        }
        chr_major = MAJOR(dev);
    }

    cdev_init(&chr_cdev, &chr_fops);
    cdev_add(&chr_cdev, dev, CHR_MODULE_MAX);


    /* minor CHR_MODULE_MAX ~ minor 2 * CHR_MODULE_MAX -1 --> chr_fops2*/
    dev = MKDEV(chr_major, CHR_MODULE_MAX);
    ret = register_chrdev_region(dev, CHR_MODULE_MAX, CHR_MODULE_NAME);
    if (ret)
    {
        PRINT_ERR("reg chrdev region fail \n");
        return -1;
    }
    cdev_init(&chr_cdev2, &chr_fops2);
    cdev_add(&chr_cdev2, dev, CHR_MODULE_MAX);


    chr_class = class_create(THIS_MODULE, CHR_MOUDLE_CLASS_NAME);
    if (!chr_class)
    {
        PRINT_ERR("class create fail \n");
        goto err_free_chr;
    }

    for (i = 0; i < CHR_MODULE_MAX + 3; i++)
    {
        if (!(class_device_create(chr_class, NULL, MKDEV(chr_major, i), NULL, "chr_dev%d", i)))
        {
            PRINT_ERR("class dev create fail:%d \n", i);
            goto err_free_class;
        }
    }

    PRINT_INFO("%s init ok\n", CHR_MODULE_NAME);

    return 0;

err_free_class:
    for (i -= 1; i >= 0; i--)
    {
        class_device_destroy(chr_class, MKDEV(chr_major, i));
    }

    class_destroy(chr_class);

err_free_chr:
    cdev_del(&chr_cdev);

    unregister_chrdev_region(MKDEV(chr_major, 0), 2);

    return -1;
}

static __exit void new_chr_exit(void)
{
    class_destroy(chr_class);

    cdev_del(&chr_cdev);
    cdev_del(&chr_cdev2);

    unregister_chrdev_region(MKDEV(chr_major, 0), CHR_MODULE_MAX);
    unregister_chrdev_region(MKDEV(chr_major, CHR_MODULE_MAX), CHR_MODULE_MAX);


    PRINT_INFO("%s exit \n", CHR_MODULE_NAME);
}

module_init(new_chr_init);
module_exit(new_chr_exit);

MODULE_LICENSE ("GPL");


