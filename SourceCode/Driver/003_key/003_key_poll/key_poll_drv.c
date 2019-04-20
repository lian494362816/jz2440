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

#define KEY_POLL_MODULE_NAME "key poll module"
#define KEY_POLL_CLASS_NAME "key_poll_class"
#define KEY_POLL_CLASS_DEV_NAME ("key_polls")
#define KEY_POLL_MAJOR (0)

#define SIZE_OF(x) (sizeof(x) / sizeof(x[0]))

struct key_irq_desc {
    unsigned int irq;
    irqreturn_t (*handler)(int, void *);
    unsigned long irq_flags;
    char *name;
};

struct key_pin_desc{
    int pin;
    int value;
};

static unsigned int key_poll_major = 0;
static struct class *key_poll_class = NULL;
static struct class_device *key_poll_class_dev = NULL;
static int key_poll_val = 0;
static int key_poll_waitq_cond = 0;

DECLARE_WAIT_QUEUE_HEAD(key_poll_waitq);


static irqreturn_t key_irq_handler(int irq, void *dev_t)
{
    int val = 0;
    struct key_pin_desc *p = (struct key_pin_desc *)dev_t;

    val = s3c2410_gpio_getpin(p->pin);

    if (val)
    {
        key_poll_val = p->value | 0x1;
    }
    else
    {
        key_poll_val = p->value;
    }

    key_poll_waitq_cond = 1;

    wake_up_interruptible(&key_poll_waitq);

    return IRQ_RETVAL(0);
}

struct key_irq_desc key_poll_irqs []= {
    {
        .irq = IRQ_EINT0,
        .handler = key_irq_handler,
        .irq_flags = IRQT_BOTHEDGE,
        .name = "KEY1",
    },

    {
        .irq = IRQ_EINT2,
        .handler = key_irq_handler,
        .irq_flags = IRQT_BOTHEDGE,
        .name = "KEY2",
    },

    {
        .irq = IRQ_EINT11,
        .handler = key_irq_handler,
        .irq_flags = IRQT_BOTHEDGE,
        .name = "KEY3",
    },

    {
        .irq = IRQ_EINT19,
        .handler = key_irq_handler,
        .irq_flags = IRQT_BOTHEDGE,
        .name = "KEY4",
    },
};

struct key_pin_desc key_poll_pins [] = {
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

static int key_poll_open(struct inode *inode, struct file *file)
{
    int ret = 0;
    int i = 0;

    if (SIZE_OF(key_poll_irqs)!= SIZE_OF(key_poll_pins))
    {
        PRINT_ERR("key irqs's count is not equle key pins's count\n");
        return -1;
    }

    for (i = 0; i < SIZE_OF(key_poll_irqs); i++)
    {
        ret = request_irq(key_poll_irqs[i].irq, key_poll_irqs[i].handler, key_poll_irqs[i].irq_flags,
                      key_poll_irqs[i].name, &key_poll_pins[i]);

        if (ret < 0)
        {
            PRINT_ERR("request fail:%d\n", i);
            goto free_irq;
        }
    }

    return 0;

free_irq:
    for (i -= 1; i >= 0; i--)
    {
        free_irq(key_poll_irqs[i].irq, &key_poll_pins[i]);
    }
    return -1;
}

static size_t key_poll_read(struct file *file, char __user *userbuf, size_t count, loff_t *off)
{
    /* wait_event_interruptible(key_poll_waitq, key_poll_waitq_cond); */
    /* key_poll_waitq_cond = 0; */

    copy_to_user(userbuf, &key_poll_val, sizeof(key_poll_val));

    return sizeof(key_poll_val);
}

static int key_poll_close(struct inode *inode, struct file *file)
{
    int i = 0;
    for (i = 0; i < SIZE_OF(key_poll_irqs); i++)
    {
        free_irq(key_poll_irqs[i].irq, &key_poll_pins[i]);
    }

    return 0;
}

static unsigned int key_poll_poll(struct file *file, struct poll_table_struct *tab)
{
    unsigned int mask = 0;

    poll_wait(file, &key_poll_waitq, tab);

    if (key_poll_waitq_cond)
    {
        mask |= POLLIN | POLLRDNORM;
        key_poll_waitq_cond = 0;
    }

    return mask;
}

static struct file_operations key_poll_fops = {
    .owner = THIS_MODULE,
    .open = key_poll_open,
    .read = key_poll_read,
    .release = key_poll_close,
    .poll = key_poll_poll,
};

int key_poll_init(void)
{
    key_poll_major = register_chrdev(KEY_POLL_MAJOR, KEY_POLL_MODULE_NAME, &key_poll_fops);
    if (key_poll_major < 0)
    {
        PRINT_ERR("%s init fail\n", KEY_POLL_MODULE_NAME);
        return -1;
    }

    key_poll_class =  class_create(THIS_MODULE, KEY_POLL_CLASS_NAME);
    if (IS_ERR(key_poll_class))
    {
        PRINT_ERR("%s create fail\n", KEY_POLL_CLASS_NAME);
        goto free_chrdev;
    }

    key_poll_class_dev = class_device_create(key_poll_class, NULL, MKDEV(key_poll_major, 0), NULL, KEY_POLL_CLASS_DEV_NAME);
    if (IS_ERR(key_poll_class_dev))
    {
        PRINT_ERR("%s create fail\n", KEY_POLL_CLASS_DEV_NAME);
        goto free_class;
    }

    PRINT_INFO("%s init\n", KEY_POLL_MODULE_NAME);

    return 0;

free_class:
    class_unregister(key_poll_class);

free_chrdev:
    unregister_chrdev(key_poll_major, KEY_POLL_MODULE_NAME);
    return -1;
}

void key_poll_exit(void)
{
    class_device_unregister(key_poll_class_dev);

    class_unregister(key_poll_class);

    unregister_chrdev(key_poll_major, KEY_POLL_MODULE_NAME);

    PRINT_INFO("%s exit\n", KEY_POLL_MODULE_NAME);
}


module_init(key_poll_init);
module_exit(key_poll_exit);

MODULE_LICENSE("GPL");
