#include <sound/driver.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/delay.h>


#include <asm/uaccess.h>
#include <asm/io.h>
#include "my_error.h"

#define WM8976_MODULE_NAME "wm8976G"
#define WM8976_CLASS_NAME "wm8976_class"
#define WM8976_CLASS_DEV_NAME "wm8976"
#define WM8976_MAJOR (0)
#define TMP_BUFF_SIZE (4096)

//IISMODE
#define IISMOD_MPLL	  (1<<9)
#define IISMOD_SLAVE	  (1<<8)
#define IISMOD_MASTER (0 << 8)
#define IISMOD_NOXFER	  (0<<6)
#define IISMOD_RXMODE	  (1<<6)
#define IISMOD_TXMODE	  (2<<6)
#define IISMOD_TXRXMODE	  (3<<6)
#define IISMOD_LR_LLOW	  (0<<5)
#define IISMOD_LR_RLOW	  (1<<5)
#define IISMOD_IIS	  (0<<4)
#define IISMOD_MSB	  (1<<4)
#define IISMOD_8BIT	  (0<<3)
#define IISMOD_16BIT	  (1<<3)
#define IISMOD_BITMASK	  (1<<3)
#define IISMOD_256FS	  (0<<2)
#define IISMOD_384FS	  (1<<2)
#define IISMOD_16FS	  (0<<0)
#define IISMOD_32FS	  (1<<0)
#define IISMOD_48FS	  (2<<0)
#define IISMOD_FS_MASK	  (3<<0)


#define IISFCON_TXDMA	  (1<<15)
#define IISFCON_TXNORMAL  (0<<15)
#define IISFCON_RXDMA	  (1<<14)
#define IISFCON_RXNORMAL  (0<<14)
#define IISFCON_TXENABLE  (1<<13)
#define IISFCON_RXENABLE  (1<<12)
#define IISFCON_TXMASK	  (0x3f << 6)
#define IISFCON_TXSHIFT	  (6)
#define IISFCON_RXMASK	  (0x3f)
#define IISFCON_RXSHIFT	  (0)


/* WM8976*/
#define WM8976_RESET        0x0
#define WM8976_POWER1        0x1
#define WM8976_POWER2        0x2
#define WM8976_POWER3        0x3
#define WM8976_IFACE        0x4
#define WM8976_COMP            0x5
#define WM8976_CLOCK        0x6
#define WM8976_ADD            0x7
#define WM8976_GPIO            0x8
#define WM8976_JACK1        0x9
#define WM8976_DAC            0xa
#define WM8976_DACVOLL        0xb
#define WM8976_DACVOLR      0xc
#define WM8976_JACK2        0xd
#define WM8976_ADC            0xe
#define WM8976_ADCVOL        0xf
#define WM8976_EQ1            0x12
#define WM8976_EQ2            0x13
#define WM8976_EQ3            0x14
#define WM8976_EQ4            0x15
#define WM8976_EQ5            0x16
#define WM8976_DACLIM1        0x18
#define WM8976_DACLIM2        0x19
#define WM8976_NOTCH1        0x1b
#define WM8976_NOTCH2        0x1c
#define WM8976_NOTCH3        0x1d
#define WM8976_NOTCH4        0x1e
#define WM8976_ALC1            0x20
#define WM8976_ALC2            0x21
#define WM8976_ALC3            0x22
#define WM8976_NGATE        0x23
#define WM8976_PLLN            0x24
#define WM8976_PLLK1        0x25
#define WM8976_PLLK2        0x26
#define WM8976_PLLK3        0x27
#define WM8976_3D           0x29
#define WM8976_BEEP         0x2b
#define WM8976_INPUT        0x2c
#define WM8976_INPPGA          0x2d
#define WM8976_ADCBOOST        0x2f
#define WM8976_OUTPUT        0x31
#define WM8976_MIXL            0x32
#define WM8976_MIXR         0x33
#define WM8976_HPVOLL        0x34
#define WM8976_HPVOLR       0x35
#define WM8976_SPKVOLL      0x36
#define WM8976_SPKVOLR      0x37
#define WM8976_OUT3MIX        0x38
#define WM8976_MONOMIX      0x39

#define WM8976_CACHEREGNUM     58



struct iis_reg_t {
    unsigned int iiscon;
    unsigned int iismod;
    unsigned int iispsr;
    unsigned int iisfcon;
    unsigned int iisfifo;
};

enum wm8976_control_pin_e {
    SCLK = 2,
    SDIN = 3,
    CSB = 4,
};

static unsigned int wm8976_major = 0;
struct class *wm8976_class = NULL;
struct class_device *wm8976_class_dev = NULL;

/*
0x56000040
GPE4  I2SDO    -- DACDAT
GPE3  I2SDI    -- ADCDAT
GPE2  CDCLK    -- MLCK
GPE1  I2SSCLK  -- BCLK
GPE0  I2SLRCK  -- LRC
*/
static volatile unsigned int *gpecon = NULL;
static volatile unsigned int *gpeup = NULL;

/*
0x56000010
GPB2 clock -- SCLK
GPB3 data  -- SDIN
GPB4 mode  -- CSB
*/
static volatile unsigned int *gpbcon = NULL;
static volatile unsigned int *gpbdat = NULL;
static volatile unsigned int *gpbup = NULL;
static volatile unsigned int *clkcon = NULL;//0x4C00000C
static volatile struct iis_reg_t *iis_regs  = NULL; //0x55000000
static unsigned char *tmp_buff = NULL;

static volatile unsigned int *iiscon = NULL;
static volatile unsigned int *iismod = NULL;
static volatile unsigned int *iispsr = NULL;
static volatile unsigned int *iisfcon = NULL;
static volatile unsigned int *iisfifo = NULL;


static int wm8976_gpio_init(void)
{
    gpecon = ioremap(0x56000040, 0x16);
    if (!gpecon)
    {
        PRINT_ERR("ioremap fail \n");
        return -1;
    }

    *gpecon &= ~(0x3FF);
    *gpecon |= (0x2 << 0) | (0x2 << 2) | (0x2 << 4) | (0x2 << 6) | (0x2 << 8);
    gpeup = gpecon + 8;
    *gpeup |= 0xF;

    gpbcon = ioremap(0x56000010, 0x16);
    if (!gpbcon)
    {
        iounmap(gpecon);
        PRINT_ERR("ioremap fail \n");
        return -1;
    }

    /* gpb 2,3,4 as out put */
    *gpbcon &= ~(0x3F << 4);
    *gpbcon |= (0x1 <<4) | (0x1 << 6) | (0x1 << 8);
    gpbdat = gpbcon + 4;
    gpbup = gpbcon + 8;

    *gpbup |= (0x1 << 2) | (0x1 << 4);

    PRINT_INFO("gpbcon:%x\n", *gpbcon);
    PRINT_INFO("gpecon:%x\n", *gpecon);

    return 0;
}

static void wm8976_gpio_deinit(void)
{
    iounmap(gpecon);
    iounmap(gpbcon);
}

static int wm8976_iis_init(void)
{
    unsigned long flags;

    iiscon = ioremap(0x55000000, 0x20);
    if (!iiscon)
    {
        PRINT_ERR("ioremap fail \n");
        return -1;
    }
    iismod = iiscon + 4;
    iispsr = iiscon + 8;
    iisfcon = iiscon + 12;
    iisfifo = iiscon + 16;

    clkcon = ioremap(0x4C00000C, 0x4);
    if (!clkcon)
    {
        PRINT_ERR("ioremap fail \n");
        iounmap(iis_regs);
        return -1;
    }

    local_irq_save(flags);

    //enable iis clock
    *clkcon |= (0x1 << 17);
    PRINT_INFO("clkcon:%x\n", *clkcon);

    *iismod = 0;
    *iispsr = 0;
    *iisfcon = 0;
    *iisfifo = 0;

    *clkcon &= ~(0x1 << 17);
    msleep(1);
    *clkcon |= (0x1 << 17);
    /*
    IIS prescaler [1] 0 = Disable,1 = Enable
    IIS interface [0] 0 = Disable (stop),1 = Enable (start)
    */
     *iiscon |= (0x1 << 0 ) | (0x1 << 1);

    //PRINT_INFO(" *iiscon:%d\n",  *iiscon);

    if (0) //test Tx and Rx
    {
         *iismod = IISMOD_32FS | IISMOD_256FS | IISMOD_16BIT | IISMOD_IIS | IISMOD_LR_LLOW |\
                           IISMOD_TXRXMODE | IISMOD_MASTER;

         *iisfcon = IISFCON_TXENABLE | IISFCON_RXENABLE | IISFCON_TXNORMAL | IISFCON_RXNORMAL;

    }
    else //test Tx
    {
         *iismod = IISMOD_32FS | IISMOD_256FS | IISMOD_16BIT | IISMOD_IIS | IISMOD_LR_LLOW |\
                           IISMOD_TXMODE | IISMOD_MASTER;

        *iisfcon = IISFCON_TXENABLE |IISFCON_TXNORMAL;
        //PRINT_INFO("iisfcon:%d \n", IISFCON_TXENABLE |IISFCON_TXNORMAL);
        *iismod = 137;
        *iisfcon = 8192;
        writel(137, iismod);
        //PRINT_INFO(" iismod:%d \n",  *iismod);
        PRINT_INFO("iismod readl:%d\n", readl(iismod));
        PRINT_INFO(" iisfcon:%d \n", *iisfcon);
        PRINT_INFO(" iiscon:%d\n",  *iiscon);
    }

    //codeclk = 8.192M,  codeclk = MCLK / (n + 1) = 50M / (5 + 1) = 8.333M
     *iispsr = (5 << 0) | (5 << 5);
    PRINT_INFO("iispsr:%d \n", *iispsr);

    local_irq_restore(flags);

    return 0;

}

static void wm8976_iis_deinit(void)
{
    //disable iis
     *iiscon &= ~(0x1 << 0);

    //close iis clock
    *clkcon &= ~(0x1 << 17);

    iounmap(iis_regs);
    iounmap(clkcon);
}

static int wm8976_control_pin_set(enum wm8976_control_pin_e pin, int value)
{
    if (pin < SCLK || pin > CSB)
    {
        PRINT_ERR("pin error:%d \n", pin);
        return -1;
    }

    if (value)
    {
        *gpbdat |= (0x1 << pin);
    }
    else
    {
        *gpbdat &= ~(0x1 << pin);
    }

    return 0;
}

static void wm8976_write_data(unsigned int reg, unsigned int data)
{
    unsigned short tmp = 0;
    int i = 0;
    /*
    [15:9] reg address, [8:0] reg data
    */
    tmp = ((reg & 0x7F) << 9) | (data & 0x1FF);

    wm8976_control_pin_set(CSB, 1);
    wm8976_control_pin_set(SCLK, 1);
    wm8976_control_pin_set(SDIN, 1);
    udelay(1);
    while(i < 16)
    {
        if (tmp & 0x8000)
        {
            wm8976_control_pin_set(SCLK, 0);
            wm8976_control_pin_set(SDIN, 1);
            udelay(1);
            wm8976_control_pin_set(SCLK, 1);
        }
        else
        {
            wm8976_control_pin_set(SCLK, 0);
            wm8976_control_pin_set(SDIN, 0);
            udelay(1);
            wm8976_control_pin_set(SCLK, 1);
        }

        tmp <<= 1;
        i ++;
    }
    wm8976_control_pin_set(CSB, 0);
    udelay(1);
    wm8976_control_pin_set(CSB, 1);
    wm8976_control_pin_set(SCLK, 1);

}

static int wm8976_hw_init(void)
{
    //reset chip
    wm8976_write_data(0, 0);

    wm8976_write_data(0x3, 0x6f);

    wm8976_write_data(0x1, 0x1f);//biasen,BUFIOEN.VMIDSEL=11b
    wm8976_write_data(0x2, 0x185);//ROUT1EN LOUT1EN, inpu PGA enable ,ADC enable

    wm8976_write_data(0x6, 0x0);//SYSCLK=MCLK
    wm8976_write_data(0x4, 0x10);//16bit
    wm8976_write_data(0x2B,0x10);//BTL OUTPUT
    wm8976_write_data(0x9, 0x50);//Jack detect enable
    wm8976_write_data(0xD, 0x21);//Jack detect
    wm8976_write_data(0x7, 0x01);//Jack detect
    if (0)
    {
        wm8976_write_data(WM8976_RESET, 0);//reset
        wm8976_write_data(WM8976_POWER1, 0x1f);//biasen,BUFIOEN.VMIDSEL=11b
        wm8976_write_data(WM8976_POWER2, 0x180);//Enable LOUT1 ROUT1
        wm8976_write_data(WM8976_POWER3, 0x3);//Enable LDAC RDAC
        wm8976_write_data(WM8976_IFACE, 0x10);//16bit, I2smode
        wm8976_write_data(WM8976_CLOCK, 0);//SYSCLK=MCLK
        wm8976_write_data(WM8976_ADD, 2);//32K sample rate
    }
    wm8976_write_data(WM8976_HPVOLL, 0x1F);
    wm8976_write_data(WM8976_HPVOLR, 0x1F);

    return 0;
}



static int wm8976_open(struct inode *inode, struct file *file)
{
    tmp_buff = kmalloc(TMP_BUFF_SIZE, GFP_KERNEL);
    if (!tmp_buff)
    {
        PRINT_ERR("kmalloc fail \n");
        return -ENOMEM;
    }

    return 0;
}

static int wm8976_close(struct inode *inode, struct file *file)
{
    kfree(tmp_buff);

    return 0;
}

static ssize_t wm8976_write(struct file *file, const char __user *buffer, size_t count , loff_t *ppos)
{
    int i = 0;
    unsigned short tmp = 0;
    if (count > TMP_BUFF_SIZE)
    {
        PRINT_ERR("size to large:%d \n", count);
        return -ENOMEM;
    }

    copy_from_user(tmp_buff, buffer, count);
    //PRINT_INFO("size:%d \n", count);

    while(i < count)
    {
        //transmist FIFO empty
        if (!( *iiscon & (0x1 << 7)) )
        {
            memcpy(&tmp , tmp_buff + i, 2);
            *iisfifo = tmp;
        }
        else
        {
            msleep(1);
            printk("msleep \n");
        }
        //printk("fifo count:%d \n", ( iisfcon >> 6) & 0x3F);
        printk("fifo count:%d \n",  *iisfcon);
        i += 2;
    }

    return 0;
}

static struct file_operations wm8976_fops = {
    .owner = THIS_MODULE,
    .open = wm8976_open,
    .write =  wm8976_write,
    .release = wm8976_close,
};

static __init int wm8976_init(void)
{
    wm8976_major = register_chrdev(WM8976_MAJOR, WM8976_MODULE_NAME, &wm8976_fops);
    if (!wm8976_major)
    {
        PRINT_ERR("%s reg chrdev fail \n", WM8976_MODULE_NAME);
        return -1;
    }

    wm8976_class = class_create(THIS_MODULE, WM8976_CLASS_NAME);
    if (!wm8976_class)
    {
        PRINT_ERR("%s class create fail \n", WM8976_CLASS_NAME);
        goto err_free_chrdev;
    }

    wm8976_class_dev = class_device_create(wm8976_class, NULL, MKDEV(wm8976_major, 0), NULL, WM8976_CLASS_DEV_NAME);
    if (!wm8976_class_dev)
    {
        PRINT_ERR("%s class dev create fail \n", WM8976_CLASS_DEV_NAME);
        goto err_free_class_dev;
    }

    /* iis and wm8976 gpio set*/
    wm8976_gpio_init();

    /* iis set*/
    wm8976_iis_init();

    /* wm8976 hw init */
    wm8976_hw_init();

    PRINT_INFO("%s init ok \n", WM8976_MODULE_NAME);

    return 0;

err_free_class_dev:
    class_unregister(wm8976_class);

err_free_chrdev:
    unregister_chrdev(wm8976_major, WM8976_MODULE_NAME);

    return -1;
}

static __exit void wm8976_exit(void)
{
    class_device_unregister(wm8976_class_dev);

    class_unregister(wm8976_class);

    unregister_chrdev(wm8976_major, WM8976_MODULE_NAME);

    wm8976_gpio_deinit();

    wm8976_iis_deinit();

    PRINT_INFO("%s exit \n", WM8976_MODULE_NAME);
}

module_init(wm8976_init);
module_exit(wm8976_exit);

MODULE_LICENSE ("GPL");
