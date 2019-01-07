#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/irq.h>
#include <linux/io.h>
#include <linux/list.h>
#include <asm/gpio.h>
#include "my_error.h"

#define LED_DRV_MODULE_NAME "led drv module"
#define LED_DRV_MODULE_MAJOR (0)
#define LED_DRV_CLASS_NAME "led_drv_class"
#define LED_DRV_CLASS_DEV_NAME "led_drv"

struct gppin {
    int pin_num;
    struct list_head list;
};

static LIST_HEAD(pin_list);


static unsigned int volatile *gpcon = NULL;
static unsigned int volatile *gpdat = NULL;

static struct class *led_drv_class = NULL;
static struct class_device *led_drv_class_dev = NULL;
static int led_drv_major = 0;

static int led_drv_open(struct inode *inode, struct file *file)
{
    struct gppin *pos;

    list_for_each_entry(pos, &pin_list, list)
    {
         /* set to output mode */
        *gpcon &= ~(0x3 << (pos->pin_num * 2));
        *gpcon |= (0x1 << (pos->pin_num * 2));

        /* turn on led */
        *gpdat &= ~(0x1 << pos->pin_num );
    }

    return 0;
}

static ssize_t led_drv_read(struct file *file, char __user *buff, size_t count, loff_t *ppos)
{

    return 0;
}

static int led_drv_release(struct inode *inode, struct file *file)
{
    struct gppin *pos;

    list_for_each_entry(pos, &pin_list, list)
    {
        /* turn on led */
        *gpdat |= ~(0x1 << pos->pin_num);
    }

    return 0;
}

static struct file_operations led_drv_fops = {
    .owner = THIS_MODULE,
    .open = led_drv_open,
    .read = led_drv_read,
    .release = led_drv_release,
};

static int led_driver_probe(struct platform_device *pdev)
{
    struct resource * res = NULL;
    struct gppin *gppin = NULL;
    struct gppin *pos, *n;
    int i = 0;

    led_drv_major = register_chrdev(LED_DRV_MODULE_MAJOR, LED_DRV_MODULE_NAME, &led_drv_fops);
    if (led_drv_major < 0)
    {
        PRINT_ERR("reg chrdev fail \n");
        return -1;
    }

    led_drv_class = class_create(THIS_MODULE, LED_DRV_CLASS_NAME);
    if (IS_ERR(led_drv_class))
    {
        PRINT_ERR("class crate fail \n");
        goto err_free_chrdev;
    }

    led_drv_class_dev = class_device_create(led_drv_class, NULL, MKDEV(led_drv_major, 0), NULL, LED_DRV_CLASS_DEV_NAME);
    if (IS_ERR(led_drv_class_dev))
    {
        PRINT_ERR("class dev crate fail \n");
        goto err_free_class;
    }

    res = platform_get_resource(pdev , IORESOURCE_MEM, 0);
    if (!res)
    {
        PRINT_ERR("get res fail \n");
        goto err_free_class_dev;
    }

    /* 0x56000057-0x56000050 = 7, 7+1 = 8 */
    gpcon = ioremap(res->start, res->end - res->start + 1);
    gpdat = gpcon + 1;

    for (i = 0; i < pdev->num_resources; i++)
    {
        res = platform_get_resource(pdev, IORESOURCE_IO, i);
        if (!res)
        {
            continue;
            PRINT_ERR("get res fail \n");
            goto err_unmap;
        }

        gppin = kmalloc(sizeof(struct gppin), GFP_KERNEL);
        if (!gppin)
        {
           PRINT_ERR("kmalloc fail \n");
           goto err_free_pin;
        }
        gppin->pin_num = res->start;
        list_add_tail(&gppin->list, &pin_list);
    }

    PRINT_INFO("led drv prebe \n");

    return 0;

err_free_pin:
    list_for_each_entry_safe(pos, n, &pin_list, list)
    {
        list_del(&pos->list);
        kfree(pos);
    }

err_unmap:
    iounmap(gpcon);

err_free_class_dev:
    class_device_unregister(led_drv_class_dev);

err_free_class:
    class_unregister(led_drv_class);

err_free_chrdev:
    unregister_chrdev(led_drv_major, LED_DRV_MODULE_MAJOR);

    return -1;

}

static int led_driver_remove(struct platform_device *pdev)
{
    struct gppin *pos, *n;

    class_device_unregister(led_drv_class_dev);

    class_unregister(led_drv_class);

    unregister_chrdev(led_drv_major, LED_DRV_MODULE_NAME);

    iounmap(gpcon);

    list_for_each_entry_safe(pos, n, &pin_list, list)
    {
        list_del(&pos->list);
        kfree(pos);
    }

    PRINT_INFO("led drv remove \n");

    return 0;
}

static struct platform_driver led_driver = {
    .probe = led_driver_probe,
    .remove = led_driver_remove,
    .driver = {
        .name = "my_led",
    }
};

static int led_driver_init(void)
{
    return platform_driver_register(&led_driver);
}

static void led_driver_exit(void)
{
    platform_driver_unregister(&led_driver);
}

module_init(led_driver_init);
module_exit(led_driver_exit);

MODULE_LICENSE("GPL");

