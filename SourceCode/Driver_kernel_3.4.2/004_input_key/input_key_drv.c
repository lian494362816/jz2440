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
#include <linux/gpio_keys.h>
#include <asm/gpio.h>
#include <mach/gpio.h>
#include <linux/sched.h>
#include "my_error.h"

/* reference Documentation/input/input-programming */

#define INPUT_KEY_MODULE_NAME "input key module"
#define SIZE_OF(x) (sizeof(x) / sizeof(x[0]))
#define PRESS (1)
#define RELEASE (0)

struct input_key_irq_t {
    unsigned int irq;
    irqreturn_t (*handler)(int, void *);
    unsigned long irq_flags;
    const char *name;
    int pin;
    int value;
};

static struct input_dev *input = NULL;
static struct timer_list input_key_timer;
static struct input_key_irq_t *input_key_irq = NULL;


static irqreturn_t input_key_handler(int irq, void *data)
{
    input_key_irq = (struct input_key_irq_t *)data;

    /* 10ms */
    mod_timer(&input_key_timer, jiffies + (HZ/100));

    return IRQ_RETVAL(0);
}

static void input_key_timer_handler(unsigned long data)
{
    struct input_key_irq_t *p = input_key_irq;
    int value = 0;

    if (NULL == p)
    {
        return ;
    }

    value = s3c2410_gpio_getpin(p->pin);

    /* release */
    if (value)
    {
        input_report_key(input, p->value, RELEASE);

    }
    else /* press */
    {
        input_report_key(input, p->value, PRESS);
    }

    input_sync(input);
}



/* can't not use static*/
struct input_key_irq_t input_key_irqs [] = {
    {
        .irq = IRQ_EINT0,
        .handler = input_key_handler,
        .irq_flags = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
        .name = "KEY1",
        .pin = S3C2410_GPF(0),
        .value = KEY_L,
    },

    {
        .irq = IRQ_EINT2,
        .handler = input_key_handler,
        .irq_flags = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
        .name = "KEY2",
        .pin = S3C2410_GPF(2),
        .value = KEY_S,
    },

    {
        .irq = IRQ_EINT11,
        .handler = input_key_handler,
        .irq_flags = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
        .name = "KEY3",
        .pin = S3C2410_GPG(3),
        .value = KEY_LEFTSHIFT,
    },

    {
        .irq = IRQ_EINT19,
        .handler = input_key_handler,
        .irq_flags = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
        .name = "KEY4",
        .pin = S3C2410_GPG(11),
        .value = KEY_ENTER,
    }
};

static int input_key_init(void)
{
    int ret = 0;
    int i = 0;

    /* set input dev */
    input = input_allocate_device();
    if (!input)
    {
        PRINT_ERR("allocate input fail \n");
        return -ENOMEM;
    }

    set_bit(EV_KEY, input->evbit);
    /* autorepeat */
    set_bit(EV_REP, input->evbit);

    set_bit(KEY_L, input->keybit);
    set_bit(KEY_S, input->keybit);
    set_bit(KEY_LEFTSHIFT, input->keybit);
    set_bit(KEY_ENTER, input->keybit);

    ret = input_register_device(input);
    if (ret)
    {
        PRINT_ERR("reg input dev fail \n");
        goto err_free_input_dev;
    }

    /* set timer */
    init_timer(&input_key_timer);
    input_key_timer.function = input_key_timer_handler;
    add_timer(&input_key_timer);

    /* set iqr */
    for (i = 0; i < SIZE_OF(input_key_irqs); i++)
    {
        ret = request_irq(input_key_irqs[i].irq, input_key_irqs[i].handler, input_key_irqs[i].irq_flags, input_key_irqs[i].name, &input_key_irqs[i]);
        if (ret)
        {
            PRINT_ERR("request irq fail:%d \n", i);
            goto err_free_irq;
        }
    }

    PRINT_INFO("%s init ok\n", INPUT_KEY_MODULE_NAME);

    return 0;

err_free_irq:
    input_unregister_device(input);

    for (i -= 1; i >= 0; i--)
    {
        free_irq(input_key_irqs[i].irq, &input_key_irqs[i]);
    }

err_free_input_dev:
    input_free_device(input);
    return -1;
}

static void input_key_exit(void)
{
    int i = 0;

    for (i = 0; i < SIZE_OF(input_key_irqs); i++)
    {
        free_irq(input_key_irqs[i].irq, &input_key_irqs[i]);
    }

    del_timer(&input_key_timer);

    input_unregister_device(input);

    input_free_device(input);

    PRINT_INFO("%s exit \n", INPUT_KEY_MODULE_NAME);
}

module_init(input_key_init);
module_exit(input_key_exit);

MODULE_LICENSE("GPL");

