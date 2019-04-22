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
#include <linux/dma-mapping.h>
#include <linux/fb.h>
#include <asm/uaccess.h>
#include <asm/gpio.h>
#include "my_error.h"

/* ADCCON Register Bits */
#define S3C2410_ADCCON_ECFLG		(1<<15)
#define S3C2410_ADCCON_PRSCEN		(1<<14)
#define S3C2410_ADCCON_PRSCVL(x)	(((x)&0xFF)<<6)
#define S3C2410_ADCCON_PRSCVLMASK	(0xFF<<6)
#define S3C2410_ADCCON_SELMUX(x)	(((x)&0x7)<<3)
#define S3C2410_ADCCON_MUXMASK		(0x7<<3)
#define S3C2410_ADCCON_STDBM		(1<<2)
#define S3C2410_ADCCON_READ_START	(1<<1)
#define S3C2410_ADCCON_ENABLE_START	(1<<0)
#define S3C2410_ADCCON_STARTMASK	(0x3<<0)

/* ADCTSC Register Bits */
#define S3C2410_ADCTSC_UD_SEN       (1<<8)
#define S3C2410_ADCTSC_YM_SEN		(1<<7)
#define S3C2410_ADCTSC_YP_SEN		(1<<6)
#define S3C2410_ADCTSC_XM_SEN		(1<<5)
#define S3C2410_ADCTSC_XP_SEN		(1<<4)
#define S3C2410_ADCTSC_PULL_UP_DISABLE	(1<<3)
#define S3C2410_ADCTSC_AUTO_PST		(1<<2)
#define S3C2410_ADCTSC_XY_PST(x)	(((x)&0x3)<<0)

/* ADCDAT0 Bits */
#define S3C2410_ADCDAT0_UPDOWN		(1<<15)
#define S3C2410_ADCDAT0_AUTO_PST	(1<<14)
#define S3C2410_ADCDAT0_XY_PST		(0x3<<12)
#define S3C2410_ADCDAT0_XPDATA_MASK	(0x03FF)

/* ADCDAT1 Bits */
#define S3C2410_ADCDAT1_UPDOWN		(1<<15)
#define S3C2410_ADCDAT1_AUTO_PST	(1<<14)
#define S3C2410_ADCDAT1_XY_PST		(0x3<<12)
#define S3C2410_ADCDAT1_YPDATA_MASK	(0x03FF)

#define TS_COUNT (1)
#define TS_TIMER_MOD (jiffies + HZ/200)

struct adc_reg_t {
    unsigned int adccon;  //0x58000000
    unsigned int adctsc;
    unsigned int adcdly;
    unsigned int adcdat0;
    unsigned int adcdat1;
    unsigned int adcupdn; //0x58000014
};

struct ts_data_t {
    unsigned int x;
    unsigned int y;
    unsigned int count;
};

static volatile unsigned int *clock = NULL;
static volatile struct adc_reg_t *adc_regs = NULL;
static struct timer_list ts_timer;
static struct input_dev *ts_input_dev = NULL;
static struct ts_data_t ts_data = {
    .x = 0,
    .y = 0,
    .count = 0,
};

static void ts_wait_up_mode(void)
{
    adc_regs->adctsc = S3C2410_ADCTSC_YM_SEN|\
                       S3C2410_ADCTSC_YP_SEN|\
                       S3C2410_ADCTSC_XP_SEN|\
                       S3C2410_ADCTSC_XY_PST(0x3)|\
                       S3C2410_ADCTSC_UD_SEN;

}

static void ts_wait_down_mode(void)
{
    adc_regs->adctsc = S3C2410_ADCTSC_YM_SEN|\
                       S3C2410_ADCTSC_YP_SEN|\
                       S3C2410_ADCTSC_XP_SEN|\
                       S3C2410_ADCTSC_XY_PST(0x3);

}

static void ts_start_adc(void)
{
    adc_regs->adctsc = S3C2410_ADCTSC_YM_SEN|\
                       S3C2410_ADCTSC_YP_SEN|\
                       S3C2410_ADCTSC_XP_SEN|\
                       S3C2410_ADCTSC_AUTO_PST|\
                       S3C2410_ADCTSC_PULL_UP_DISABLE|\
                       S3C2410_ADCTSC_XY_PST(0);

    adc_regs->adccon |= S3C2410_ADCCON_ENABLE_START;
}

/*  return 1 touch screen release
    return 0 touch screen pressed
*/
static int ts_get_status(void)
{
    int up_down = 0;

    up_down = (adc_regs->adcdat0 & S3C2410_ADCDAT0_UPDOWN) && (adc_regs->adcdat1 & S3C2410_ADCDAT1_UPDOWN);

    return up_down;
}

static irqreturn_t ts_irq_handle(int irq ,void *data)
{
    int up_down = 0;

    up_down = ts_get_status();

    /* up */
    if (up_down)
    {
        //PRINT_INFO("ts up \n");
        ts_wait_down_mode();
        ts_data.count = 0;
        ts_data.x = 0;
        ts_data.y = 0;
        input_report_key(ts_input_dev, BTN_TOUCH, 0);
        input_report_abs(ts_input_dev, ABS_PRESSURE, 0);
        input_sync(ts_input_dev);
    }
    else /* down */
    {
        //PRINT_INFO("ts down \n");
        ts_wait_up_mode();
        ts_start_adc();
    }

    return IRQ_HANDLED;
}

static irqreturn_t adc_irq_handle(int irq, void *data)
{
    int up_down = 0;

    ts_data.x += adc_regs->adcdat0 & S3C2410_ADCDAT0_XPDATA_MASK;
    ts_data.y += adc_regs->adcdat1 & S3C2410_ADCDAT1_YPDATA_MASK;
    ts_data.count ++;

    up_down = ts_get_status();

    /* up */
    if (up_down)
    {
        ts_wait_down_mode();
    }
    else /* down */
    {
        ts_wait_up_mode();
        mod_timer(&ts_timer, TS_TIMER_MOD);
    }

    return IRQ_HANDLED;
}

static void ts_timer_handle(unsigned long data)
{
    int up_down = 0;

    up_down = ts_get_status();

    if (!up_down)
    {
        if (TS_COUNT == ts_data.count)
        {
            ts_data.x /= TS_COUNT;
            ts_data.y /= TS_COUNT;
            /* printk("x:%8d, y:%8d \n", ts_data.x, ts_data.y); */
            input_report_abs(ts_input_dev, ABS_X, ts_data.x);
            input_report_abs(ts_input_dev, ABS_Y, ts_data.y);
            /* !0->pressed, 0->release*/
            input_report_key(ts_input_dev, BTN_TOUCH, 1);
            input_report_abs(ts_input_dev, ABS_PRESSURE, 1);
            input_sync(ts_input_dev);

            ts_data.count = 0;
            ts_data.x = 0;
            ts_data.y = 0;
        }
        ts_start_adc();
    }

}

static int ts_init(void)
{
    int ret = 0;
    clock = ioremap(0x4C00000C, 4);
    if (!clock)
    {
        PRINT_ERR("ioremap fail \n");
        return -1;
    }

    adc_regs = ioremap(0x58000000, sizeof(struct adc_reg_t));
    if (!adc_regs)
    {
        PRINT_ERR("ioremap fail \n");
        goto err_unmap_clock;
    }

    /* enable adc clock */
    *clock |= (0x1 <<15);

    adc_regs->adccon = S3C2410_ADCCON_PRSCEN | S3C2410_ADCCON_PRSCVL(49);
    adc_regs->adcdly = 0xFFFF;

    ts_wait_down_mode();

    ret = request_irq(IRQ_TC, ts_irq_handle, IRQF_SAMPLE_RANDOM, "ts_irq", NULL);
    if (ret < 0)
    {
        PRINT_ERR("request irq fail:%d \n", ret);
        goto err_free_adc_regs;
    }

    if (request_irq(IRQ_ADC, adc_irq_handle, IRQF_SAMPLE_RANDOM, "adc_irq", NULL))
    {
        PRINT_ERR("request irq fail \n");
        goto err_free_irq_ts;
    }

    ts_input_dev = input_allocate_device();
    if (!ts_input_dev)
    {
        PRINT_ERR("input alloc fail \n");
        goto err_free_irq_adc;
    }

    set_bit(EV_SYN, ts_input_dev->evbit);
    set_bit(EV_KEY, ts_input_dev->evbit);
    set_bit(EV_ABS, ts_input_dev->evbit);
    set_bit(BTN_TOUCH, ts_input_dev->keybit);
    input_set_abs_params(ts_input_dev, ABS_X, 0, 0x3FF, 0, 0);
    input_set_abs_params(ts_input_dev, ABS_Y, 0, 0x3FF, 0, 0);
    input_set_abs_params(ts_input_dev, ABS_PRESSURE, 0, 1, 0, 0);

    if (input_register_device(ts_input_dev))
    {
        PRINT_ERR("input reg fail \n");
        goto err_free_input;
    }

    init_timer(&ts_timer);
    ts_timer.function = ts_timer_handle;
    add_timer(&ts_timer);

    return 0;

err_free_input:
    input_free_device(ts_input_dev);

err_free_irq_adc:
    free_irq(IRQ_ADC, NULL);

err_free_irq_ts:
    free_irq(IRQ_TC, NULL);

err_free_adc_regs:
    iounmap(adc_regs);

err_unmap_clock:
    iounmap(clock);
    return -1;

}

static void ts_exit(void)
{
    /*disable adc clock */
    *clock &= ~(0x1 <<15);

    free_irq(IRQ_TC, NULL);
    free_irq(IRQ_ADC, NULL);
    del_timer(&ts_timer);

    input_unregister_device(ts_input_dev);
    input_free_device(ts_input_dev);

    iounmap(adc_regs);
    iounmap(clock);
}

module_init(ts_init);
module_exit(ts_exit);

MODULE_LICENSE("GPL");

