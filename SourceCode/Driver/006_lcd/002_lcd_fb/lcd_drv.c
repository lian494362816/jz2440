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
static 	struct fb_info	   *lcd_fb_info = NULL;
static u32 lcd_pseudo_palette[16];

static void gpio_set(unsigned int *gpio, unsigned int value, unsigned int mask)
{
    unsigned int tmp = 0;
    tmp = *gpio & (~mask);
    *gpio = tmp | value;
}

static inline unsigned int lcd_chan_to_field(unsigned int chan, struct fb_bitfield *bf)
{
    chan &= 0xffff;
    chan >>= 16 - bf->length;
    return chan << bf->offset;
}

static int lcd_setcolreg(unsigned regno, unsigned red, unsigned green, unsigned blue,
                              unsigned transp, struct fb_info *info)
{
    unsigned int val;

    if (regno < 16)
    {
        val  = lcd_chan_to_field(red,   &info->var.red);
        val |= lcd_chan_to_field(green, &info->var.green);
        val |= lcd_chan_to_field(blue,  &info->var.blue);

        lcd_pseudo_palette[regno] = val;
    }
    else
    {
        PRINT_WRN("regno error:%d\n", regno);
        return -1;
    }
    return 0;
}


static struct fb_ops lcd_fb_ops = {
    .owner		= THIS_MODULE,
    .fb_setcolreg	= lcd_setcolreg,
    .fb_fillrect	= cfb_fillrect,
    .fb_copyarea	= cfb_copyarea,
    .fb_imageblit	= cfb_imageblit,
};

int lcd_open(struct inode *inode, struct file *file)
{
    return 0;
}

int lcd_release(struct inode *inode, struct file *file)
{
    return 0;
}

ssize_t lcd_read(struct file *file, char __user *buff, size_t count, loff_t *ppos)
{

    return 0;
}

ssize_t lcd_write(struct file *file, const char __user *buff, size_t count, loff_t *ppos)
{
    //copy_from_user(lcd_vir_addr, buff, count);
    //memset(lcd_vir_addr, (unsigned char )*buff, LCD_X_RES * LCD_Y_RES * 2);

    return 0;
}

static struct file_operations lcd_fops = {
    .owner = THIS_MODULE,
    .open = lcd_open,
    .release = lcd_release,
    .read = lcd_read,
    .write = lcd_write,
};

int lcd_init(void)
{
    int ret = 0;
        /* 16bpp */
    lcd_vir_addr = dma_alloc_writecombine(NULL, LCD_X_RES * LCD_Y_RES * 2, &lcd_phy_addr, GFP_KERNEL);
    if (!lcd_vir_addr)
    {
        PRINT_ERR("alloc fail \n");
        return 0;
    }

    lcd_fb_info = framebuffer_alloc(0, NULL);
    if (!lcd_fb_info)
    {
        PRINT_ERR("alloc fb fail \n");
        goto err_free_dma;
    }

    /* remap gpio */
    if (!(gpiob_con = ioremap(0x56000010, 8)))
    {
        PRINT_ERR("ioremap fail \n");
        goto err_free_fb;
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

    /* set lcd frame buffer info */
    lcd_fb_info->fix.accel = FB_ACCEL_NONE;
    strcpy(lcd_fb_info->fix.id, LCD_MODULE_NAME);
    lcd_fb_info->fix.line_length = LCD_X_RES * 2;
    lcd_fb_info->fix.mmio_len = 0;
    lcd_fb_info->fix.mmio_start = 0;
    lcd_fb_info->fix.smem_len = LCD_X_RES * LCD_Y_RES * 2;
    lcd_fb_info->fix.smem_start = lcd_phy_addr;
    lcd_fb_info->fix.type = FB_TYPE_PACKED_PIXELS;
    lcd_fb_info->fix.type_aux = 0;
    lcd_fb_info->fix.visual = FB_VISUAL_TRUECOLOR;
    lcd_fb_info->fix.xpanstep  = 0;
    lcd_fb_info->fix.ypanstep  = 0;
    lcd_fb_info->fix.ywrapstep = 0;


    lcd_fb_info->var.xres =          LCD_X_RES;
    lcd_fb_info->var.yres =          LCD_Y_RES;
    lcd_fb_info->var.xres_virtual =  LCD_X_RES;
    lcd_fb_info->var.yres_virtual =  LCD_Y_RES;
    lcd_fb_info->var.xoffset =       0;
    lcd_fb_info->var.yoffset =       0;

    lcd_fb_info->var.bits_per_pixel = 16;
    lcd_fb_info->var.grayscale =      0;

    /* RGB 565 */
    lcd_fb_info->var.red.length =       5;
    lcd_fb_info->var.red.msb_right =    0;
    lcd_fb_info->var.red.offset =       11;

    lcd_fb_info->var.green.length =     6;
    lcd_fb_info->var.green.msb_right =  0;
    lcd_fb_info->var.green.offset =     5;

    lcd_fb_info->var.blue.length =      5;
    lcd_fb_info->var.blue.msb_right =   0;
    lcd_fb_info->var.blue.offset =      0;

    lcd_fb_info->var.transp.length =    0;
    lcd_fb_info->var.transp.msb_right = 0;
    lcd_fb_info->var.transp.offset =    0;

    lcd_fb_info->var.nonstd =   0;
    lcd_fb_info->var.activate = FB_ACTIVATE_NOW;
    lcd_fb_info->var.height =   LCD_Y_RES;
    lcd_fb_info->var.width =    LCD_X_RES;
    //lcd_fb_info->var.accel_flags =

    /*
    lcd_fb_info->var.pixclock =            0;
    lcd_fb_info->var.left_margin =
    lcd_fb_info->var.right_margin =
    lcd_fb_info->var.upper_margin =
    lcd_fb_info->var.lower_margin =
    lcd_fb_info->var.hsync_len =
    lcd_fb_info->var.vsync_len =
    lcd_fb_info->var.sync =
    lcd_fb_info->var.vmode =
    lcd_fb_info->var.rotate =
    */

    lcd_fb_info->fbops =          &lcd_fb_ops;
    lcd_fb_info->flags =          FBINFO_FLAG_DEFAULT;
    lcd_fb_info->pseudo_palette = lcd_pseudo_palette;
    lcd_fb_info->screen_size =    LCD_X_RES * LCD_Y_RES * 2;
    lcd_fb_info->screen_base =    lcd_vir_addr;

    PRINT_INFO("fb info set done \n");

    ret = register_framebuffer(lcd_fb_info);
    if (ret)
    {
        PRINT_ERR("reg fb fail:%d \n", ret);
        goto err_free_lcd_reg;
    }

    return 0;

err_free_lcd_reg:
    iounmap(lcd_regs);

err_free_gpiod:
    iounmap(gpiod_con);

err_free_gpioc:
    iounmap(gpioc_con);

err_free_gpiob:
    iounmap(gpiob_con);

err_free_fb:
    framebuffer_release(lcd_fb_info);

err_free_dma:
    dma_free_writecombine(NULL, LCD_X_RES * LCD_Y_RES * 2, lcd_vir_addr, lcd_phy_addr);

    return -1;
#if 0
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
#endif
}

void lcd_exit(void)
{
#if 0
    class_device_unregister(lcd_class_dev);

    class_unregister(lcd_class);

    unregister_chrdev(lcd_major, LCD_MODULE_NAME);
#endif
        /* turn off background light */
    *gpiob_dat &= ~(0x1);

    lcd_regs->lcdcon1 &= ~(S3C2440_LCDCON1_ENVID(1));
    lcd_regs->lcdcon5 &= ~(S3C2440_LCDCON5_PWREN(1));

    unregister_framebuffer(lcd_fb_info);

    dma_free_writecombine(NULL, LCD_X_RES * LCD_Y_RES * 2, lcd_vir_addr, lcd_phy_addr);

    framebuffer_release(lcd_fb_info);

    iounmap(lcd_regs);
    iounmap(gpiod_con);
    iounmap(gpioc_con);
    iounmap(gpiob_con);

    PRINT_INFO("%s exit \n", LCD_MODULE_NAME);
}

module_init(lcd_init);
module_exit(lcd_exit);

MODULE_LICENSE("GPL");

