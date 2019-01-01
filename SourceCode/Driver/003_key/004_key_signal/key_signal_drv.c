#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>
#include "my_error.h"

#define KEY_SIGNAL_MODULE_NAME "key signal"
#define KEY_SIGNAL_CLASS_NAME "key_signal_cleass"
#define KEY_SIGNAL_CLASS_DEV_NAME "key_signals"
#define KEY_SIGNAL_MAJOR (0)

#define SIZE_OF(x) (sizeof(x) / sizeof(x[0]))

static struct class *key_signal_class = NULL;
static struct class_device *key_signal_class_dev = NULL;
static unsigned int key_signal_major = 0;
static int key_val = 0;

static struct fasync_struct *key_fasync;

struct key_signal_desc {
    unsigned int irq;
    irqreturn_t (*handler)(int, void *);
    unsigned long flag;
    char *name;
};

struct key_signal_pin {
    int pin;
    int value;
};

static irqreturn_t key_signal_handler(int irq ,void *dev_t)
{
    int value = 0;
    struct key_signal_pin *p = (struct key_signal_pin *)dev_t;

    value = s3c2410_gpio_getpin(p->pin);

    if (value)
    {
        key_val = p->value | 0x1;
    }
    else
    {
        key_val = p->value;
    }

    kill_fasync(&key_fasync, SIGIO, POLLIN);

    return IRQ_RETVAL(0);
}

static struct key_signal_desc key_signals [] = {
    {
        .irq = IRQ_EINT0,
        .handler = key_signal_handler,
        .flag = IRQT_BOTHEDGE,
        .name = "KEY1",
    },

    {
        .irq = IRQ_EINT2,
        .handler = key_signal_handler,
        .flag = IRQT_BOTHEDGE,
        .name = "KEY2",
    },

    {
        .irq = IRQ_EINT11,
        .handler = key_signal_handler,
        .flag = IRQT_BOTHEDGE,
        .name = "KEY3",
    },

    {
        .irq = IRQ_EINT19,
        .handler = key_signal_handler,
        .flag = IRQT_BOTHEDGE,
        .name = "KEY4",
    },
};

/* here can't use static*/
struct key_signal_pin key_pins [] = {
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

static int key_signal_open(struct inode *inode, struct file *file)
{
    int i = 0;
    int ret = 0;

    if (SIZE_OF(key_signals) != SIZE_OF(key_pins))
    {
        PRINT_ERR("signals is not equal pins \n");
        return -1;
    }

    if (IRQ_EINT0 != key_signals[0].irq)
    {
        PRINT_WRN("irq not equal\n");
    }

    if (key_signal_handler != key_signals[0].handler)
    {
        PRINT_WRN("handler not equal\n");
    }

    if (IRQT_BOTHEDGE != key_signals[0].flag)
    {
        PRINT_WRN("flag not equal\n");
    }

    if ("KEY1" != key_signals[0].name)
    {
        PRINT_WRN("name not equal \n");
    }

    for (i = 0; i < SIZE_OF(key_signals); i++)
    {
        ret = request_irq(key_signals[i].irq, key_signals[i].handler, key_signals[i].flag, key_signals[i].name, &key_pins[i]);

        if (ret < 0)
        {
            PRINT_ERR("reg irq fail:%d \n", i);
            goto freeirq;
        }
    }

    return 0;

freeirq:
    for (i -= 1; i >= 0; i--)
    {
        free_irq(key_signals[i].irq, &key_pins[i]);
    }

    return -1;

}

static ssize_t key_signal_read (struct file *file,  char __user *userbuf, size_t count, loff_t *off)
{
    copy_to_user(userbuf, &key_val, sizeof(key_val));

    return sizeof(key_val);
}

static int key_signal_close(struct inode *inode, struct file *file)
{
    int i = 0;

    for (i = 0; i < SIZE_OF(key_signals); i++)
    {
        free_irq(key_signals[i].irq, &key_pins[i]);
    }

    return 0;
}

static int key_signal_fasync(int fd, struct file *file, int on)
{
    int ret = 0;

    ret = fasync_helper(fd, file, on, &key_fasync);
    if (ret < 0)
    {
        PRINT_WRN("fasync helper fail \n");
        return -1;
    }

    return 0;
}


static struct file_operations key_signal_fops = {
    .owner = THIS_MODULE,
    .open = key_signal_open,
    .read = key_signal_read,
    .release = key_signal_close,
    .fasync = key_signal_fasync,
};

int key_signal_init(void)
{
    key_signal_major = register_chrdev(KEY_SIGNAL_MAJOR, KEY_SIGNAL_MODULE_NAME, &key_signal_fops);
    if (key_signal_major < 0)
    {
        PRINT_ERR("reg %s fail \n", KEY_SIGNAL_MODULE_NAME);
        return -1;
    }

    key_signal_class = class_create(THIS_MODULE, KEY_SIGNAL_CLASS_NAME);
    if (IS_ERR(key_signal_class))
    {
        PRINT_ERR("create %s fail \n", KEY_SIGNAL_CLASS_NAME);
        goto free_chrdev;
    }

    key_signal_class_dev = class_device_create(key_signal_class, NULL, MKDEV(key_signal_major, 0), NULL, KEY_SIGNAL_CLASS_DEV_NAME);
    if (IS_ERR(key_signal_class_dev))
    {
        PRINT_ERR("create %s fail \n", KEY_SIGNAL_CLASS_DEV_NAME);
        goto free_class;
    }

    PRINT_INFO("%s init ok \n", KEY_SIGNAL_MODULE_NAME);

    return 0;

free_class:
    class_unregister(key_signal_class);

free_chrdev:
    unregister_chrdev(key_signal_major, KEY_SIGNAL_MODULE_NAME);
    return -1;

}

void key_signal_exit(void)
{
    class_device_unregister(key_signal_class_dev);

    class_unregister(key_signal_class);

    unregister_chrdev(key_signal_major, KEY_SIGNAL_MODULE_NAME);

    PRINT_INFO("%s exit \n", KEY_SIGNAL_MODULE_NAME);
}

module_init(key_signal_init);
module_exit(key_signal_exit);

MODULE_LICENSE("GPL");



