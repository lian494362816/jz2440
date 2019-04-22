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
#include <linux/spinlock.h>




//#include <asm/arch/regs-gpio.h>
//#include <asm/hardware.h>
#include <mach/gpio.h>
#include <linux/sched.h>

#include "my_error.h"

#define KEY_LOCK_MODULE_NAME "key lock module"
#define KEY_LOCK_CLASS_NAME "key_lock_class"
#define KEY_LOCK_CLASS_DEV_NAME "key_locks"
#define KEY_LOCK_MAJOR (0)

#define SIZE_OF(x) (sizeof(x) / sizeof(x[0]))

struct key_lock_irq_t{
    unsigned int irq;
    irqreturn_t (*handler)(int irq, void *dev_t);
    unsigned long irq_flags;
    char *name;
};

struct key_lock_pin_t{
    int pin;
    int value;
};

static DECLARE_WAIT_QUEUE_HEAD(key_lock_wait);
struct class *key_lock_class = NULL;
struct device *key_lock_class_dev = NULL;
static unsigned int key_lock_major = 0;
static int key_val = 0;
static int key_lock_wait_val = 0;

static atomic_t key_open = ATOMIC_INIT(1);

//static atomic_t key_open;

static irqreturn_t key_lock_handler(int irq, void *dev_t)
{
    struct key_lock_pin_t *p = (struct key_lock_pin_t *)dev_t;
    int val = 0;

    val = s3c2410_gpio_getpin(p->pin);

    /* key release  */
    if (val)
    {
        key_val = p->value | 0x1;
    }
    else /* key press */
    {
        key_val=  p->value;
    }

    key_lock_wait_val = 1;
    wake_up_interruptible(&key_lock_wait);

    return IRQ_RETVAL(0);
}

struct key_lock_irq_t key_lock_irqs [] = {
    {
        .irq = IRQ_EINT0,
        .handler = key_lock_handler,
        .irq_flags = IRQF_TRIGGER_RISING	| IRQF_TRIGGER_FALLING,
        .name = "KEY1",
    },

    {
        .irq = IRQ_EINT2,
        .handler = key_lock_handler,
        .irq_flags = IRQF_TRIGGER_RISING	| IRQF_TRIGGER_FALLING,
        .name = "KEY2",
    },

    {
        .irq = IRQ_EINT11,
        .handler = key_lock_handler,
        .irq_flags = IRQF_TRIGGER_RISING	| IRQF_TRIGGER_FALLING,
        .name = "KEY3",
    },

    {
        .irq = IRQ_EINT19,
        .handler = key_lock_handler,
        .irq_flags = IRQF_TRIGGER_RISING	| IRQF_TRIGGER_FALLING,
        .name = "KEY4",
    },
};

struct key_lock_pin_t key_lock_pins [] = {
    {
        .pin = S3C2410_GPF(0),
        .value = 0x10,
    },

    {
        .pin = S3C2410_GPF(2),
        .value = 0x20,
    },

    {
        .pin = S3C2410_GPG(3),
        .value = 0x30,
    },

    {
        .pin = S3C2410_GPG(11),
        .value = 0x40,
    },
};

static int key_lock_open(struct inode *inode, struct file *file)
{
    int i = 0;
    int ret = 0;

    if (!atomic_dec_and_test(&key_open))
    {
        atomic_inc(&key_open);
        return -EBUSY;
    }

    if (SIZE_OF(key_lock_irqs) != SIZE_OF(key_lock_pins))
    {
        PRINT_ERR("key irqs != key pins");
        return -1;
    }

    for (i = 0; i < SIZE_OF(key_lock_irqs); i++)
    {
        ret = request_irq(key_lock_irqs[i].irq, key_lock_irqs[i].handler, key_lock_irqs[i].irq_flags, key_lock_irqs[i].name, &key_lock_pins[i]);
        if (ret < 0)
        {
            PRINT_ERR("request irq fail:%d \n", i);
            goto free_irqs;
        }
    }

    return 0;

free_irqs:
    for (i -= 1; i >= 0; i--)
    {
        free_irq(key_lock_irqs[i].irq, &key_lock_pins[i]);
    }
    return -1;
}

static ssize_t key_lock_read(struct file *file, char __user *userbuf, size_t count, loff_t * off)
{
    wait_event_interruptible(key_lock_wait, key_lock_wait_val);
    key_lock_wait_val = 0;

    copy_to_user(userbuf, &key_val, sizeof(key_val));

    return sizeof(key_val);
}

static int key_lock_close(struct inode *inode, struct file *file)
{
    int i = 0;

    for (i = 0; i < SIZE_OF(key_lock_irqs); i++)
    {
        free_irq(key_lock_irqs[i].irq, &key_lock_pins[i]);
    }

    atomic_inc(&key_open);

    return 0;
}


static struct file_operations key_lock_fops = {
    .owner = THIS_MODULE,
    .open = key_lock_open,
    .release = key_lock_close,
    .read = key_lock_read,
};

int key_lock_init(void)
{
    key_lock_major = register_chrdev(KEY_LOCK_MAJOR, KEY_LOCK_MODULE_NAME, &key_lock_fops);
    if (key_lock_major < 0)
    {
        PRINT_ERR("%s reg fail \n", KEY_LOCK_MODULE_NAME);
        return -1;
    }

    key_lock_class = class_create(THIS_MODULE, KEY_LOCK_CLASS_NAME);
    if (IS_ERR(key_lock_class))
    {
        PRINT_ERR("%s create fial \n", KEY_LOCK_CLASS_NAME);
        goto free_chr;
    }

    key_lock_class_dev = device_create(key_lock_class, NULL, MKDEV(key_lock_major, 0), NULL, KEY_LOCK_CLASS_DEV_NAME);
    if (IS_ERR(key_lock_class_dev))
    {
        PRINT_ERR("%s create fail \n", KEY_LOCK_CLASS_DEV_NAME);
        goto free_class;
    }

    PRINT_INFO("%s init ok\n", KEY_LOCK_MODULE_NAME);

    return 0;

free_class:
    class_unregister(key_lock_class);

free_chr:
    unregister_chrdev(key_lock_major, KEY_LOCK_MODULE_NAME);
    return -1;
}

void key_lock_exit(void)
{
    device_destroy(key_lock_class, MKDEV(key_lock_major, 0));

    class_unregister(key_lock_class);

    unregister_chrdev(key_lock_major, KEY_LOCK_MODULE_NAME);

    PRINT_INFO("%s exit \n", KEY_LOCK_MODULE_NAME);
}

module_init(key_lock_init);
module_exit(key_lock_exit);

MODULE_LICENSE ("GPL");

