#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>
#include "my_error.h"

#define KEY_IRQ_MODULE_NAME "key irq module"
#define KEY_IRQ_CLASS_NAME  "key_irq_class"
#define KEY_IRQ_CLASS_DEVICE_NAME "key_irqs"
#define KEY_IRQ_MAJOR 0

struct key_irq_desc{
    unsigned int irq;
    irqreturn_t  (*handler)(int, void *);
    unsigned long irqflags;
    const char *devname;
};

struct pin_desc{
    unsigned int pin;
    unsigned int value;
};

static DECLARE_WAIT_QUEUE_HEAD(key_irq_waitq);


static unsigned int key_irq_major = 0;
struct class *key_irq_class = NULL;
struct class_device *key_irq_class_dev = NULL;
static volatile int key_irq_waitq_val = 0;
static unsigned int key_val = 0;
/*
         KEY1    KYE2    KEY3    KEY4
release  0x11    0x21    0x31    0x41
press    0x10    0x20    0x30    0x40
*/

static irqreturn_t key_irq_handle (int irq_num, void *dev_id)
{
    struct pin_desc *p = (struct pin_desc *)dev_id;
    int value = 0;

    value = s3c2410_gpio_getpin(p->pin);

    /* pin release */
    if (value)
    {
        key_val = p->value | 0x1;
    }
    else /*  pin press */
    {
       key_val = p->value;
    }

    key_irq_waitq_val = 1;
    wake_up_interruptible(&key_irq_waitq);

    return IRQ_RETVAL(0);
}


struct pin_desc pins_desc[] = {
    {
        .pin = S3C2410_GPF0,
        .value = 0x10,
    },

    {
        .pin = S3C2410_GPF2,
        .value = 0x20,
    },

    {
        .pin = S3C2410_GPG3,
        .value = 0x30,
    },

    {
        .pin = S3C2410_GPG11,
        .value = 0x40,
    },
};

struct key_irq_desc key_irqs_desc[] = {
    {
        .irq = IRQ_EINT0,
        .handler = key_irq_handle,
        .irqflags = IRQT_BOTHEDGE,
        .devname = "KEY1",
    },

    {
        .irq = IRQ_EINT2,
        .handler = key_irq_handle,
        .irqflags = IRQT_BOTHEDGE,
        .devname = "KEY2",
    },

    {
        .irq = IRQ_EINT11,
        .handler = key_irq_handle,
        .irqflags = IRQT_BOTHEDGE,
        .devname = "KEY3",
    },

    {
        .irq = IRQ_EINT19,
        .handler = key_irq_handle,
        .irqflags = IRQT_BOTHEDGE,
        .devname = "KEY4",
    },
};

int key_irq_open(struct inode *inode, struct file *file)
{
    int i = 0;
    int ret = 0;

    if (sizeof(key_irqs_desc) / sizeof(key_irqs_desc[0]) != sizeof(pins_desc) / sizeof(pins_desc[0]))
    {
        PRINT_ERR("key_irqs_desc's count is not equal pins_desc's count \n");
        return -1;
    }

    for (i = 0; i < sizeof(key_irqs_desc) / sizeof(key_irqs_desc[0]); i++)
    {
        ret = request_irq(key_irqs_desc[i].irq, key_irqs_desc[i].handler,
                    key_irqs_desc[i].irqflags, key_irqs_desc[i].devname, &pins_desc[i]);
    }

    if (ret < 0)
    {
        for (; i >= 0; i--)
        {
            free_irq(key_irqs_desc[i].irq, &pins_desc[i]);
        }
    }

    return 0;
}

ssize_t key_irq_read(struct file *file, char __user *usrbuf, size_t count, loff_t *off)
{
    wait_event_interruptible(key_irq_waitq, key_irq_waitq_val);
    key_irq_waitq_val = 0;

    copy_to_user(usrbuf, &key_val, sizeof(key_val));

    return sizeof(key_val);
}

int key_irq_close (struct inode *inode, struct file *file)
{
    int i = 0;

    for (i = 0; i < sizeof(key_irqs_desc) / sizeof(key_irqs_desc[0]); i++)
    {
        free_irq(key_irqs_desc[i].irq, &pins_desc[i]);
    }

    return 0;
}

struct file_operations key_irq_fops = {
    .owner = THIS_MODULE,
    .open = key_irq_open,
    .read = key_irq_read,
    .release = key_irq_close,
};

int key_irq_init(void)
{
    key_irq_major = register_chrdev(KEY_IRQ_MAJOR, KEY_IRQ_MODULE_NAME, &key_irq_fops);
    if (key_irq_major < 0)
    {
        PRINT_ERR("%s init fail\n", KEY_IRQ_MODULE_NAME);
        return -1;
    }

    /* /sys/class/key_irq_class */
    key_irq_class = class_create(THIS_MODULE, KEY_IRQ_CLASS_NAME);
    if (IS_ERR(key_irq_class))
    {
        PRINT_ERR("%s create fail\n", KEY_IRQ_CLASS_NAME);
        goto free_chrdev;
    }

    /* /dev/key_irqs */
    key_irq_class_dev = class_device_create(key_irq_class, NULL, MKDEV(key_irq_major, 0), NULL, KEY_IRQ_CLASS_DEVICE_NAME);
    if (IS_ERR(key_irq_class_dev))
    {
        PRINT_ERR("%s create fail\n", KEY_IRQ_CLASS_DEVICE_NAME);
        goto free_class;
    }

    PRINT_INFO("key irq module init\n");

    return 0;

free_class:
    class_unregister(key_irq_class);

free_chrdev:
    unregister_chrdev(key_irq_major, KEY_IRQ_MODULE_NAME);
    return -1;
}

void key_irq_exit(void)
{
    class_device_unregister(key_irq_class_dev);
    class_unregister(key_irq_class);

    unregister_chrdev(key_irq_major, KEY_IRQ_MODULE_NAME);

    PRINT_INFO("key irq module exit\n");
}

module_init(key_irq_init);
module_exit(key_irq_exit);

MODULE_LICENSE("GPL");

