#include <linux/device.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/dma-mapping.h>
#include <asm/irq.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include "my_error.h"

#define DMA_MODULE_NAME "dma module"
#define DMA_MODULE_CLASS_NAME "dma_class"
#define DMA_MODULE_CLASS_DEVICE_NAME "dma1"
#define DMA_IRQ_NAME "dma1_irq"
#define DMA_CHR_MAX 1

#define DMA0_BASE 0x4B000000
#define DMA1_BASE 0x4B000040
#define DMA2_BASE 0x4B000080
#define DMA3_BASE 0x4B0000A0
#define DMA_TEST_SIZE (1024)

#define DO_CPU (0)
#define DO_DMA (1)

/* DISRCC*/
#define DISRCC_LOC_AHB (0x0 << 1)
#define DISRCC_LOC_APB (0x1 << 1)
#define DISRCC_INC_INCREMENT (0x0 << 0)
#define DISRCC_INC_FIXED (0x1 << 0)

/* DIDSTC */
#define DIDSTC_CHK_INT_TC_REACH_0  (0x0 << 2)
#define DIDSTC_CHK_INT_AFTER_AUTO  (0x1 << 2)
#define DIDSTC_LOC_AHB             (0x0 << 1)
#define DIDSTC_LOC_APB             (0x1 << 1)
#define DIDSTC_INC_INCREMNET       (0x0 << 0)
#define DIDSTC_INC_FIXED           (0x1 << 0)

/* DCON */
#define DCON_DMD_HS_DEMAND    (0x0 << 31)
#define DCON_DMD_HS_HANDSHAKE (0x1 << 31)
#define DCON_SYNC_APB         (0x0 << 30)
#define DCON_SYNC_AHB         (0x1 << 30)
#define DCON_INT_DISABLE      (0x0 << 29)
#define DCON_INT_ENABLE       (0x1 << 29)
#define DCON_TSZ_UNIT         (0x0 << 28)
#define DCON_TSZ_BURST        (0x1 << 28)
#define DCON_SERVMODE_SIGNAL  (0x0 << 27)
#define DCON_SERVMODE_WHOLE   (0x1 << 27)

#define DCON0_HWSRCSEL_XDREQ0  (0x0 << 24)//DCON0
#define DCON0_HWSRCSEL_UART0   (0x1 << 24)
#define DCON0_HWSRCSEL_DSI     (0x2 << 24)
#define DCON0_HWSRCSEL_TIMER   (0x3 << 24)
#define DCON0_HWSRCSEL_USB_EP1 (0x4 << 24)
#define DCON0_HWSRCSEL_I2SSD0  (0x5 << 24)
#define DCON0_HWSRCSEL_PCMIN   (0x6 << 24)

#define DCON1_HWSRCSEL_XDREQ1  (0x0 << 24)//DCON1
#define DCON1_HWSRCSEL_UART1   (0x1 << 24)
#define DCON1_HWSRCSEL_I2SSDI  (0x2 << 24)
#define DCON1_HWSRCSEL_SPI     (0x3 << 24)
#define DCON1_HWSRCSEL_USB_EP2 (0x4 << 24)
#define DCON1_HWSRCSEL_PCMOUT  (0x5 << 24)
#define DCON1_HWSRCSEL_SDI     (0x6 << 24)

#define DCON2_HWSRCSEL_I2SSDO  (0x0 << 24)//DCON2
#define DCON2_HWSRCSEL_I2SSDI  (0x 1<< 24)
#define DCON2_HWSRCSEL_SDI     (0x2 << 24)
#define DCON2_HWSRCSEL_TIMER   (0x3 << 24)
#define DCON2_HWSRCSEL_USB_EP3 (0x4 << 24)
#define DCON2_HWSRCSEL_PCMIN   (0x5 << 24)
#define DCON2_HWSRCSEL_MICIN   (0x6 << 24)

#define DCON3_HWSRCSEL_UART2   (0x0 << 24)//DCON3
#define DCON3_HWSRCSEL_SDI     (0x1 << 24)
#define DCON3_HWSRCSEL_SPI     (0x2 << 24)
#define DCON3_HWSRCSEL_TIMER   (0x3 << 24)
#define DCON3_HWSRCSEL_USB_EP4 (0x4 << 24)
#define DCON3_HWSRCSEL_MICIN   (0x5 << 24)
#define DCON3_HWSRCSEL_PCMOUT  (0x6 << 24)

#define DCON_SWHW_SEL_SW (0x0 << 23)
#define DCON_SWHW_SEL_HW (0x1 << 23)

#define DCON_RELOAD_ENABLE  (0x0 << 22)
#define DCON_RELOAD_DISABLE (0x1 << 22)

#define DCON_DSZ_BYTE      (0x0 << 20)
#define DCON_DSZ_HALF_WORD (0x1 << 20)
#define DCON_DSZ_WORD      (0x2 << 20)
#define DCON_TC(x)         ((x) & 0xFFFFF)

/* DMASKTRIG */
#define DCON_DMASKTRIG_STOP    (0x1 << 2)
#define DCON_DMASKTRIG_OFF     (0x0 << 1)
#define DCON_DMASKTRIG_ON      (0x1 << 1)
#define DCON_DMASKTRIG_SW_TRIG (0x1 << 0)

struct dma_reg_t {
    unsigned int disrc;
    unsigned int disrcc;
    unsigned int didst;
    unsigned int didstc;
    unsigned int dcon;
    unsigned int dstat;
    unsigned int dcsrc;
    unsigned int dcdst;
    unsigned int dmasktrig;
};

static unsigned int dma_major = 0;
static struct class *dma_class = NULL;
static struct class_device *dma_device_class = NULL;
static struct cdev dma_cdev;
static volatile struct dma_reg_t *dma_regs = NULL;
static unsigned char *src_vir = NULL;
static dma_addr_t src_phy = 0;
static unsigned char *dst_vir = NULL;
static dma_addr_t dst_phy = 0;
static int dma_wait_con = 0;

DECLARE_WAIT_QUEUE_HEAD(dma_wait_queue);


static int dma_open(struct inode *inode, struct file *file)
{
    //PRINT_INFO("%s open \n", DMA_MODULE_NAME);

    dma_wait_con = 0;

    return 0;
}


static int dma_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    int i = 0;
    memset(src_vir, 0xA, DMA_TEST_SIZE);
    memset(dst_vir, 0, DMA_TEST_SIZE);

    switch(cmd)
    {
        case DO_CPU:
            memcpy(dst_vir, src_vir, DMA_TEST_SIZE);
            for (i = 0; i < DMA_TEST_SIZE; i++)
            {
                if (0xA != dst_vir[i])
                {
                    PRINT_INFO("cpu copy fail \n");
                    return 0;
                }
            }
            //PRINT_INFO("cpu copy ok \n");

            break;

        case DO_DMA:
            dma_regs->dmasktrig = DCON_DMASKTRIG_ON | DCON_DMASKTRIG_SW_TRIG;
            wait_event_interruptible(dma_wait_queue, dma_wait_con);
            dma_wait_con = 0;
            for (i = 0; i < DMA_TEST_SIZE; i++)
            {
                if (0xA != dst_vir[i])
                {
                    PRINT_INFO("dma copy fail \n");
                    return 0;
                }
            }
            //PRINT_INFO("dma copy ok \n");

            break;

        default:
            PRINT_ERR("cmd error:%d \n", cmd);
            break;
    }

    return 0;
}

static int dma_close(struct inode *inode, struct file *file)
{

    return 0;
}

static struct file_operations dma_fops = {
    .owner = THIS_MODULE,
    .open = dma_open,
    .ioctl = dma_ioctl,
    .release = dma_close,
};

static int dma_test_buff_init(void)
{
    src_vir = dma_alloc_writecombine(NULL, DMA_TEST_SIZE, &src_phy, GFP_KERNEL);
    if (!src_vir)
    {
        PRINT_ERR("dam alloc fail \n");
        return -ENOMEM;
    }

    dst_vir = dma_alloc_writecombine(NULL, DMA_TEST_SIZE, &dst_phy, GFP_KERNEL);
    if (!dst_vir)
    {
        PRINT_ERR("dam alloc fail \n");
        dma_free_writecombine(NULL, DMA_TEST_SIZE, src_vir, src_phy);
        return -ENOMEM;
    }

    return 0;
}

static void dma_test_buff_free(void)
{
    dma_free_writecombine(NULL, DMA_TEST_SIZE, src_vir, src_phy);
    dma_free_writecombine(NULL, DMA_TEST_SIZE, dst_vir, dst_phy);
}

static irqreturn_t dma_irq_handle(int irq, void *data)
{
    dma_wait_con = 1;
    wake_up_interruptible(&dma_wait_queue);

    return IRQ_HANDLED;
}

static int dma_reg_init(void)
{
    dma_regs = ioremap(DMA1_BASE, sizeof(struct dma_reg_t));
    if (!dma_regs)
    {
        PRINT_ERR("ioremap fail \n");
        return -ENOMEM;
    }

    dma_regs->disrc = src_phy;
    dma_regs->disrcc = DISRCC_INC_INCREMENT | DISRCC_LOC_AHB;
    dma_regs->didst = dst_phy;
    dma_regs->didstc = DIDSTC_CHK_INT_AFTER_AUTO | DIDSTC_INC_INCREMNET |DIDSTC_LOC_AHB;

#if 1
    /* use burst mode*/
    dma_regs->dcon = DCON_DMD_HS_HANDSHAKE | DCON_SYNC_AHB | DCON_INT_ENABLE | DCON_TSZ_BURST |\
                     DCON_SERVMODE_WHOLE | DCON1_HWSRCSEL_XDREQ1 | DCON_SWHW_SEL_SW | DCON_RELOAD_DISABLE |\
                     DCON_DSZ_BYTE | DCON_TC(DMA_TEST_SIZE / 4);
#else
    /* use unit mode */
    dma_regs->dcon = DCON_DMD_HS_HANDSHAKE | DCON_SYNC_AHB | DCON_INT_ENABLE | DCON_TSZ_UNIT |\
                 DCON_SERVMODE_WHOLE | DCON1_HWSRCSEL_XDREQ1 | DCON_SWHW_SEL_SW | DCON_RELOAD_DISABLE |\
                 DCON_DSZ_BYTE | DCON_TC(DMA_TEST_SIZE);
#endif
    /* start transfer*/
    //dma_regs->dmasktrig = DCON_DMASKTRIG_ON | DCON_DMASKTRIG_SW_TRIG;

    return 0;
}

static int dma_irq_init(void)
{
    int ret = 0;

    ret = request_irq(IRQ_DMA1, dma_irq_handle, 0, DMA_IRQ_NAME, NULL);
    if (ret)
    {
        PRINT_ERR("request irq fail \n");
        return -EBUSY;
    }

    return 0;
}

static void dma_irq_deinit(void)
{
    free_irq(IRQ_DMA1, NULL);
}

static __init int dma_init(void)
{
    dev_t dev;
    int ret = 0;
    if (dma_major)
    {
        dev = MKDEV(dma_major, 0);
        ret = register_chrdev_region(dev, DMA_CHR_MAX, DMA_MODULE_NAME);
        if (ret)
        {
            PRINT_ERR("reg chrdev region fail \n");
            return -1;
        }
    }
    else
    {

        ret = alloc_chrdev_region(&dev, 0, DMA_CHR_MAX, DMA_MODULE_NAME);
        if (ret)
        {
            PRINT_ERR("allo chrdev region fail \n");
            return -1;
        }
        dma_major = MAJOR(dev);
    }

    cdev_init(&dma_cdev, &dma_fops);

    ret = cdev_add(&dma_cdev, dev, DMA_CHR_MAX);
    if (ret)
    {
        PRINT_ERR("cdev add fail \n");
        goto err_free_chrdev_reg;
    }

    dma_class = class_create(THIS_MODULE, DMA_MODULE_CLASS_NAME);
    if (!dma_class)
    {
        PRINT_ERR("class create fail \n");
        goto err_free_cdev;
    }

    dma_device_class = class_device_create(dma_class, NULL, dev, NULL, DMA_MODULE_CLASS_DEVICE_NAME);
    if (!dma_device_class)
    {
        PRINT_ERR("class device create fail \n");
        goto err_free_class;
    }

    dma_test_buff_init();

    dma_reg_init();

    dma_irq_init();

    PRINT_INFO("%s init ok \n", DMA_MODULE_NAME);

    return 0;

err_free_class:
    class_destroy(dma_class);

err_free_cdev:
    cdev_del(&dma_cdev);

err_free_chrdev_reg:
    unregister_chrdev_region(dev , DMA_CHR_MAX);

    return -1;
}


static __exit void dma_exit (void)
{
    unregister_chrdev_region(MKDEV(dma_major, 0), DMA_CHR_MAX);

    cdev_del(&dma_cdev);

    class_destroy(dma_class);

    dma_test_buff_free();

    dma_irq_deinit();

    PRINT_INFO("%s exit \n", DMA_MODULE_NAME);
}

module_init(dma_init);
module_exit(dma_exit);

MODULE_LICENSE ("GPL");
