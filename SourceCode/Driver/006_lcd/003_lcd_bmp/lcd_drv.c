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


#define LCD_MODULE_NAME "lcd module"
#define LCD_MAJOR (0)
#define LCD_CLASS_NAME "lcd_class"
#define LCD_CLASS_DEV_NAME "lcd"

/* lcd con1 */
#define S3C2440_LCDCON1_CLKVAL(x)   ((x) << 8)
#define S3C2440_LCDCON1_MMODE(x)    ((x) << 7)
#define S3C2440_LCDCON1_PNRMODE_4BIT_DUAL    (0x0 << 5) //for STN
#define S3C2440_LCDCON1_PNRMODE_4BIT_SINGLE  (0x1 << 5) //for STN
#define S3C2440_LCDCON1_PNRMODE_8BIT_SINGLE  (0x2 << 5) //for STN
#define S3C2440_LCDCON1_PNRMODE_TFT          (0x3 << 5)

#define S3C2440_LCDCON1_BPPMODE_1BPP_STN (0x0 << 1)
#define S3C2440_LCDCON1_BPPMODE_2BPP_STN (0x1 << 1)
#define S3C2440_LCDCON1_BPPMODE_4BPP_STN (0x2 << 1)
#define S3C2440_LCDCON1_BPPMODE_8BPP_STN (0x3 << 1)
#define S3C2440_LCDCON1_BPPMODE_PACK_12BPP_STN (0x4 << 1)
#define S3C2440_LCDCON1_BPPMODE_UNPACK_12BPP_STN (0x5 << 1)
#define S3C2440_LCDCON1_BPPMODE_16BPP_STN (0x6 << 1)
#define S3C2440_LCDCON1_BPPMODE_1BPP_TFT (0x8 << 1)
#define S3C2440_LCDCON1_BPPMODE_2BPP_TFT (0x9 << 1)
#define S3C2440_LCDCON1_BPPMODE_4BPP_TFT (0xA << 1)
#define S3C2440_LCDCON1_BPPMODE_8BPP_TFT (0xB << 1)
#define S3C2440_LCDCON1_BPPMODE_16BPP_TFT (0xC << 1)
#define S3C2440_LCDCON1_BPPMODE_24BPP_TFT (0xD << 1)
#define S3C2440_LCDCON1_ENVID(x)    ((x) << 0)

/* lcd con2 */
#define S3C2440_LCDCON2_VBPD(x)     (((x) - 1) << 24)
#define S3C2440_LCDCON2_LINEVAL(x)  (((x) - 1) << 14)
#define S3C2440_LCDCON2_VFPD(x)     (((x) - 1) << 6)
#define S3C2440_LCDCON2_VSPW(x)     (((x) -1) << 0)

/* lcd con3 */
#define S3C2440_LCDCON3_HBPD(x)     (((x) -1) << 19)
#define S3C2440_LCDCON3_HOZVAL(x)   (((x) -1) << 8)
#define S3C2440_LCDCON3_HFPD(x)     (((x) -1) << 0)

/* lcd con4 */
#define S3C2440_LCDCON4_MVAL(x)     ((x) << 8) //for STN
#define S3C2440_LCDCON4_HSPW(x)     (((x) -1) << 0)

/* lcd con5 */
#define S3C2440_LCDCON5_BPP24BL_LSB     (0 << 12)
#define S3C2440_LCDCON5_BPP24BL_MSB     (0x1 << 12)

#define S3C2440_LCDCON5_FRM565_555      (0 << 11)
#define S3C2440_LCDCON5_FRM565_565      (0x1 << 11)

#define S3C2440_LCDCON5_INVVCLK(x)     ((x) << 10)
#define S3C2440_LCDCON5_INVVLINE(x)    ((x) << 9)
#define S3C2440_LCDCON5_INVVFRAME(x)   ((x) << 8)
#define S3C2440_LCDCON5_INVVD(x)       ((x) << 7)
#define S3C2440_LCDCON5_INVVDEN(x)     ((x) << 6)
#define S3C2440_LCDCON5_INVPWREN(x)    ((x) << 5)
#define S3C2440_LCDCON5_INVLEND(x)     ((x) << 4)
#define S3C2440_LCDCON5_PWREN(x)       ((x) << 3)
#define S3C2440_LCDCON5_ENLEND(x)      ((x) << 2)
#define S3C2440_LCDCON5_BSWP(x)        ((x) << 1)
#define S3C2440_LCDCON5_HWSWP(x)       ((x) << 0)

/* lcd saddr1 */
#define S3C2440_LCDSADDR1_LCDBANK_S3C2410_LCDBANK(x)   ((x) >> 1)

/* lcd saddr2 */
//#define S3C2440_LCDSADDR2_LCDBASEL(x)  ((((x) >> 1) + 1) & 0x1FFFFF)
#define S3C2440_LCDSADDR2_LCDBASEL(x)  (((x) >> 1) & 0x1FFFFF)


/* lcd saddr3 */
#define S3C2440_LCDSADDR3_OFFSIZE(x)   ((x) << 11)
#define S3C2440_LCDSADDR3_PAGEWIDTH(x) ((x) << 0)

#define LCD_X_RES (480)
#define LCD_Y_RES (272)

struct lcd_reg_t {
    unsigned int lcdcon1;     //0X4D000000
    unsigned int lcdcon2;     //0X4D000004
    unsigned int lcdcon3;     //0X4D000008
    unsigned int lcdcon4;     //0X4D00000C
    unsigned int lcdcon5 ;    //0X4D000010
    unsigned int lcdsaddr1;   //0X4D000014
    unsigned int lcdsaddr2;   //0X4D000018
    unsigned int lcdsaddr3;   //0X4D00001C
    unsigned int redlut ;     //0X4D000020
    unsigned int greenlut;    //0X4D000024
    unsigned int bluelut;     //0X4D000028
    unsigned int reserved[9]; //36Byte not use
    unsigned int dithmode;    //0X4D00004C
    unsigned int tpal;        //0X4D000050
    unsigned int lcdintpnd;   //0X4D000054
    unsigned int lcdsrcpnd;   //0X4D000058
    unsigned int lcdintmsk;   //0X4D00005C
    unsigned int tconsel;     //0X4D000060
};

static struct class *lcd_class = NULL;
static struct class_device *lcd_class_dev = NULL;
static int lcd_major = 0;
static unsigned int lcd_phy_addr = 0;
static void *lcd_vir_addr = NULL;

static volatile unsigned int *gpiob_con = NULL;
static volatile unsigned int *gpiob_dat = NULL;
static volatile unsigned int *gpioc_con = NULL;
static volatile unsigned int *gpiod_con = NULL;
static volatile struct lcd_reg_t *lcd_regs = NULL;

static void gpio_set(unsigned int *gpio, unsigned int value, unsigned int mask)
{
    unsigned int tmp = 0;
    tmp = *gpio & (~mask);
    *gpio = tmp | value;
}

int lcd_open(struct inode *inode, struct file *file)
{
                     /* 16bpp */
    lcd_vir_addr = dma_alloc_writecombine(NULL, LCD_X_RES * LCD_Y_RES * 2, &lcd_phy_addr, GFP_KERNEL);
    if (!lcd_vir_addr)
    {
        PRINT_ERR("alloc fail \n");
        return -1;
    }

    /* remap gpio */
    if (!(gpiob_con = ioremap(0x56000010, 8)))
    {
        PRINT_ERR("ioremap fail \n");
        goto err_free_dma;
    }
    gpiob_dat = gpiob_con + 1;

    if (!(gpioc_con = ioremap(0x56000020, 4)))
    {
        PRINT_ERR("ioremap fail \n");
        goto err_free_gpiob;
    }

    if (!(gpiod_con = ioremap(0x56000030, 4)))
    {
        PRINT_ERR("ioremap fail \n");
        goto err_free_gpioc;
    }

    if ( !(lcd_regs = ioremap(0x4D000000, sizeof(struct lcd_reg_t))) )
    {
        PRINT_ERR("ioremap fail \n");
        goto err_free_gpiod;
    }

    /* set gpio */
    *gpiob_con &= ~(0x3);
    *gpiob_con |= 0x1;

    gpio_set(gpioc_con, 0xAAAAAAAA, 0x0000FC03);
    gpio_set(gpiod_con, 0xAAAAAAAA, 0);

    /* set lcd reg */
    lcd_regs->lcdcon1 = S3C2440_LCDCON1_CLKVAL(4) | \
                        S3C2440_LCDCON1_MMODE(0)  | \
                        S3C2440_LCDCON1_PNRMODE_TFT|\
                        S3C2440_LCDCON1_BPPMODE_16BPP_TFT;
                       // S3C2440_LCDCON1_ENVID(1);


    lcd_regs->lcdcon2 = S3C2440_LCDCON2_VBPD(2)|\
                        S3C2440_LCDCON2_LINEVAL(LCD_Y_RES)|\
                        S3C2440_LCDCON2_VFPD(2)|\
                        S3C2440_LCDCON2_VSPW(10);

    lcd_regs->lcdcon3 = S3C2440_LCDCON3_HBPD(2)|\
                        S3C2440_LCDCON3_HOZVAL(LCD_X_RES)|\
                        S3C2440_LCDCON3_HFPD(2);

    lcd_regs->lcdcon4 = S3C2440_LCDCON4_HSPW(41);

    lcd_regs->lcdcon5 = S3C2440_LCDCON5_BPP24BL_LSB|\
                        S3C2440_LCDCON5_FRM565_565|\
                        S3C2440_LCDCON5_INVVCLK(0)|\
                        S3C2440_LCDCON5_INVVLINE(1)|\
                        S3C2440_LCDCON5_INVVFRAME(1)|\
                        S3C2440_LCDCON5_INVVD(0)|\
                        S3C2440_LCDCON5_INVPWREN(0)|\
                        S3C2440_LCDCON5_INVVDEN(0)|\
                       /* S3C2440_LCDCON5_PWREN(1)|\ */
                        S3C2440_LCDCON5_ENLEND(1)|\
                        S3C2440_LCDCON5_BSWP(0)|\
                        S3C2440_LCDCON5_HWSWP(1);

    lcd_regs->lcdsaddr1 = S3C2440_LCDSADDR1_LCDBANK_S3C2410_LCDBANK(lcd_phy_addr);
    lcd_regs->lcdsaddr2 = S3C2440_LCDSADDR2_LCDBASEL(lcd_phy_addr +  LCD_X_RES * LCD_Y_RES * 2);

    lcd_regs->lcdcon1 |= S3C2440_LCDCON1_ENVID(1);
    lcd_regs->lcdcon5 |= S3C2440_LCDCON5_PWREN(1);

    /* turn on lcd background light */
    *gpiob_dat |= (0x1);

    return 0;

err_free_gpiod:
    iounmap(gpiod_con);

err_free_gpioc:
    iounmap(gpioc_con);

err_free_gpiob:
    iounmap(gpiob_con);

err_free_dma:
    dma_free_writecombine(NULL, LCD_X_RES * LCD_Y_RES * 2, lcd_vir_addr, lcd_phy_addr);

    return -1;
}

int lcd_release(struct inode *inode, struct file *file)
{
    /* turn off background light */
    *gpiob_dat &= ~(0x1);

    lcd_regs->lcdcon1 &= ~(S3C2440_LCDCON1_ENVID(1));
    lcd_regs->lcdcon5 &= ~(S3C2440_LCDCON5_PWREN(1));

    dma_free_writecombine(NULL, LCD_X_RES * LCD_Y_RES * 2, lcd_vir_addr, lcd_phy_addr);

    iounmap(lcd_regs);
    iounmap(gpiod_con);
    iounmap(gpioc_con);
    iounmap(gpiob_con);

    return 0;
}

ssize_t lcd_read(struct file *file, char __user *buff, size_t count, loff_t *ppos)
{

    return 0;
}

ssize_t lcd_write(struct file *file, const char __user *buff, size_t count, loff_t *ppos)
{
    if (count > LCD_X_RES * LCD_Y_RES * 2)
    {
        PRINT_ERR("max size: %d\n", LCD_X_RES * LCD_Y_RES * 2);
        return E2BIG;
    }

    if (copy_from_user(lcd_vir_addr, buff, count) )
    {
        return -EFAULT;
    }
    //memset(lcd_vir_addr, (unsigned char )*buff, LCD_X_RES * LCD_Y_RES * 2);

    return count;
}
int (*mmap) (struct file *, struct vm_area_struct *);

static int lcd_mmap(struct file *file, struct vm_area_struct *vma)
{
    unsigned long size = 0;

    size = vma->vm_end - vma->vm_start;
    PRINT_INFO("mmap size:%ldK \n", size / 1024);
    //if (remap_pfn_range(vma, vma->vm_start, virt_to_phys(lcd_vir_addr) >> PAGE_SHIFT, size, vma->vm_page_prot))
    if (remap_pfn_range(vma, vma->vm_start, lcd_phy_addr >> PAGE_SHIFT, size, vma->vm_page_prot))
    {
        PRINT_ERR("mmap fail \n");
        return -1;
    }

    return 0;
}

static struct file_operations lcd_fops = {
    .owner = THIS_MODULE,
    .open = lcd_open,
    .release = lcd_release,
    .read = lcd_read,
    .write = lcd_write,
    .mmap = lcd_mmap,
};

int lcd_init(void)
{
    lcd_major = register_chrdev(LCD_MAJOR, LCD_MODULE_NAME, &lcd_fops);
    if (lcd_major < 0)
    {
        PRINT_ERR("%s init fail \n", LCD_MODULE_NAME);
        return -1;
    }

    lcd_class = class_create(THIS_MODULE, LCD_CLASS_NAME);
    if (IS_ERR(lcd_class))
    {
        PRINT_ERR("crate class fail \n");
        goto err_free_chrdev;
    }

    lcd_class_dev = class_device_create(lcd_class, NULL, MKDEV(lcd_major, 0), NULL, LCD_CLASS_DEV_NAME);
    if (IS_ERR(lcd_class_dev))
    {
        PRINT_ERR("create class dev fail \n");
        goto err_free_class;
    }

    PRINT_INFO("%s init ok \n", LCD_MODULE_NAME);

    return 0;

err_free_class:
    class_unregister(lcd_class);

err_free_chrdev:
    unregister_chrdev(lcd_major, LCD_MODULE_NAME);

    return -1;

}

void lcd_exit(void)
{
    class_device_unregister(lcd_class_dev);

    class_unregister(lcd_class);

    unregister_chrdev(lcd_major, LCD_MODULE_NAME);

    PRINT_INFO("%s exit \n", LCD_MODULE_NAME);
}

module_init(lcd_init);
module_exit(lcd_exit);

MODULE_LICENSE("GPL");

