#include <linux/slab.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>

#include <asm/io.h>
#include <asm/sizes.h>

#include "my_error.h"


#define NAND_FLASH_MODULE_NAME "nand flash"
#define TACLS  (0)  //> 0ns
#define TWRPH0  (1) //>12ns
#define TWRPH1  (0) //>5ns

struct nand_reg_t {
    /* 0x4E000000 */
    unsigned long   nfconf  ;
    unsigned long   nfcont  ;
    unsigned long   nfcmd   ;
    unsigned long   nfaddr  ;
    unsigned long   nfdata  ;
    unsigned long   nfeccd0 ;
    unsigned long   nfeccd1 ;
    unsigned long   nfeccd  ;
    unsigned long   nfstat  ;
    unsigned long   nfestat0;
    unsigned long   nfestat1;
    unsigned long   nfmecc0 ;
    unsigned long   nfmecc1 ;
    unsigned long   nfsecc  ;
    unsigned long   nfsblk  ;
    /* 0x4E00003C */
    unsigned long   nfeblk  ;
};

struct nand_host_t {
    struct mtd_info mtd;
    struct nand_chip nand_chip;
};

//0x4C00000C
static volatile unsigned long *clkcon = NULL;
static volatile struct nand_reg_t *nand_regs = NULL;
static struct nand_host_t *nand_host = NULL;


static void nand_cmd_ctrl(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
    if (cmd == NAND_CMD_NONE)
    {
        return;
    }

    if (ctrl & NAND_CLE)
    {
        //send commond
        nand_regs->nfcmd = cmd;
    }
    else
    {
        //send addr
        nand_regs->nfaddr = cmd;
    }
}

//return 1 -> ready
//return 0 -> busy
static int nand_device_ready(struct mtd_info *mtd)
{
    return (nand_regs->nfstat & 0x1);
}

//chip -1, disable nand
//chip != -1, enable nand
static void nand_select_chip(struct mtd_info *mtd, int chip)
{
    if (-1 == chip)
    {
        nand_regs->nfcont |= (0x1 << 1);
    }
    else
    {
        nand_regs->nfcont &= ~(0x1 << 1);
    }
}

static struct mtd_partition nand_part[] = {
    [0] = {
        .name   = "bootloader",
        .size   = 0x00040000,
    	.offset	= 0,
    },
    [1] = {
        .name   = "params",
        .offset = MTDPART_OFS_APPEND,
        .size   = 0x00020000,
    },
    [2] = {
        .name   = "kernel",
        .offset = MTDPART_OFS_APPEND,
        .size   = 0x00200000,
    },
    [3] = {
        .name   = "root",
        .offset = MTDPART_OFS_APPEND,
        .size   = MTDPART_SIZ_FULL,
    }
};


static int nand_flash_init(void)
{
    struct mtd_info *mtd = NULL;
    struct nand_chip *nand_chip = NULL;

    /* enable nand clock */
    clkcon = ioremap(0x4C00000C, 4);
    if (!clkcon)
    {
        PRINT_ERR("ioremap fail \n");
        return -1;
    }

    *clkcon |= (0x1 << 4);

    /* hardware init */
    nand_regs = ioremap(0x4E000000, sizeof(struct nand_reg_t));
    if (!nand_regs)
    {
        PRINT_ERR("ioremap fail \n");
        goto err_free_clk;
    }

    //TACLS  clk = HCLK x TACLS, > 0ns
    //TWRPH0 clk = HCLK x (TWRPH1 + 1), > 12ns
    //TWRPH1 clk = HCLK x (TWRPH1 + 1), > 5ns
    nand_regs->nfconf = (TACLS << 12) | (TWRPH0 << 8) | (TWRPH1 << 4);

    //disable soft lock
    nand_regs->nfcont &= ~(0x1 << 12);
    //enable chip
    nand_regs->nfcont &= ~(0x1 << 1);
    //enable nand flash control
    nand_regs->nfcont |= (0x1);

    /* set nand_chip*/
    nand_host = kmalloc(sizeof(struct nand_host_t), GFP_KERNEL);
    if (!nand_host)
    {
        PRINT_ERR("kmalloc fail \n");
        goto err_free_nand_reg;
    }

    memset(nand_host, 0, sizeof(struct nand_host_t));

    nand_chip = &nand_host->nand_chip;
    nand_chip->IO_ADDR_R = &nand_regs->nfdata;
    nand_chip->IO_ADDR_W = &nand_regs->nfdata;
    nand_chip->cmd_ctrl = nand_cmd_ctrl;
    nand_chip->dev_ready = nand_device_ready;
    nand_chip->select_chip = nand_select_chip;
    nand_chip->ecc.mode = NAND_ECC_SOFT;
    nand_chip->chip_delay = 10;
    nand_chip->priv = nand_host;


    /* set mtd_info */
    mtd= &nand_host->mtd;
    mtd->owner = THIS_MODULE;
    mtd->priv = nand_chip;

    /* call nand_scan */
    if (nand_scan(mtd, 1))
    {
        PRINT_ERR("nand scan fail \n");
        goto err_free_nand_host;
    }

    /* add partition */

    //just one part
    //add_mtd_device(mtd);

    // more part
    if (add_mtd_partitions(mtd, nand_part, 4))
    {
        PRINT_ERR("add partition fail \n");
        goto err_free_nand_host;
    }


    PRINT_INFO("%s init ok\n", NAND_FLASH_MODULE_NAME);

    return 0;

err_free_nand_host:
    kfree(nand_host);

err_free_nand_reg:
    iounmap(nand_regs);

err_free_clk:
    /* disable nand clock */
    *clkcon &= ~(0x1 << 4);
    iounmap(clkcon);

    return -1;
}

static void nand_flash_exit(void)
{
    del_mtd_partitions(&nand_host->mtd);

    kfree(nand_host);

    iounmap(nand_regs);

    /* disable nand clock */
    *clkcon &= ~(0x1 << 4);
    iounmap(clkcon);

    PRINT_INFO("%s exit \n", NAND_FLASH_MODULE_NAME);
}

module_init(nand_flash_init);
module_exit(nand_flash_exit);

MODULE_LICENSE("GPL");
