#include "s3c2440_soc.h"

#define NAND_FLASH_DELAY_TIME (10)
#define NAND_FLASH_PAGE_SIZE (2048)
#define NAND_FLASH_BLOCK_SIZE (128 * 1024)

static void nand_flash_select(void)
{
    NFCONT &= ~(0x1 << 1);

}

static void nand_flash_deselect(void)
{
    NFCONT |= (0x1 << 1);
}

static void nand_flash_wait_ready(void)
{
    //[0], 0->busy, 1->ready

    while(!(NFSTAT & 0x1))
    {
        //nothing
    }
}

static void nand_flash_write_byte(unsigned char val)
{
    NFDATA = val;
}

static unsigned char nand_flash_data(void)
{
    return NFDATA;
}

void nand_flash_addr_byte(unsigned char addr)
{
    volatile int i = 0;
    NFADDR = addr;

    for (i = 0; i < NAND_FLASH_DELAY_TIME; i++);
}

void nand_flash_cmd(unsigned char cmd)
{
    volatile int i = 0;
    NFCMMD = cmd;

    for (i = 0; i < NAND_FLASH_DELAY_TIME; i++);
}

void nand_flash_init(void)
{
    NFCONF &= ~( (0x7 << 4) | (0x7 << 8) | (0x3 << 12) );

    //set TWRPH1, Duration = HCLK x (TWRPH1 + 1) >= 5
    NFCONF |= (0x0 << 4);

    //set TWRPH0, Duration = HCLK x ( TWRPH0 + 1 ) >=12
    NFCONF |= (0x1 << 8);

    //set TACLS, Duration = HCLK x TACLS >= 0
    NFCONF |= (0x0 << 12);


    //Disable Soft lock
    NFCONT &= ~(0x1 << 12);

    //Unlock spare area ECC
    //NFCONT &= ~(0x1 << 6);

    //Unlock Main area ECC
    //NFCONT &= ~(0x1 << 5);

    //init ECC
    NFCONT |= 0x1 <<4;

    //nand disable select
    NFCONT |= (0x1 << 1);

    //enable nand flash control
    NFCONT |= 0x1;

    //NFCONT = (1<<4) | (1<<1) | (1<<0);
}

/* nand address organize
1st Cycle A0 A1 A2 A3 A4 A5 A6 A7
2nd Cycle A8 A9 A10 A11 *L *L *L *L
3rd Cycle A12 A13 A14 A15 A16 A17 A18 A19
4th Cycle A20 A21 A22 A23 A24 A25 A26 A27
5th Cycle A28 *L *L *L *L *L *L *L
A0~A11 column addr
A13~A28 row   addr
*/

int nand_flash_read(unsigned int addr, char *buf, unsigned int size)
{
    unsigned int page_num = addr / NAND_FLASH_PAGE_SIZE;
    unsigned int column = addr & (NAND_FLASH_PAGE_SIZE - 1);
    unsigned int i = 0;

    nand_flash_select();

    while(i < size)
    {
        /* 1 send 0x00 cmd */
        nand_flash_cmd(0x00);

        /* 2 send 5 address */
        nand_flash_addr_byte(column & 0xFF);//A0~A7
        nand_flash_addr_byte((column >> 8)& 0xFF); //A8~A10, don't care A11
        nand_flash_addr_byte(page_num & 0xFF); //A12~A19
        nand_flash_addr_byte((page_num >> 8) & 0xFF);//A20~A27
        nand_flash_addr_byte((page_num >> 16) & 0xFF);//A28

        /* 3 send 0x30 cmd */
        nand_flash_cmd(0x30);

        /* 4 wait ready */
        nand_flash_wait_ready();

        /* 5 get data */
        while(column < NAND_FLASH_PAGE_SIZE && i < size)
        {
            buf[i] = nand_flash_data();
            i ++;
            column ++;
        }

        page_num ++;
        column = 0;
    }

    nand_flash_deselect();

    return 0;

}


int is_nor_flash(void)
{
    volatile unsigned int *p = (volatile unsigned int *)0;
    unsigned int tmp = *p;

    *p = 0x12345678;
    /* write succses, is run in internal sram */
    if (0x12345678 == *p)
    {
        *p = tmp;
        return 0;
    }

    /*write fail, is run in external nor flash */
    return 1;

}

void copy_to_sram(void)
{
    extern int __copy_code_start, __bss_start__;
    volatile unsigned int *src = (unsigned int *)0;
    volatile unsigned int *dst = (unsigned int *)&__copy_code_start;
    volatile unsigned int *end = (unsigned int *)&__bss_start__;
    unsigned int len = 0;

    len = ((int)&__bss_start__) - ((int) &__copy_code_start);

    if (is_nor_flash())
    {
        /* nor flash start */
        while(dst < end)
        {
            *dst++ = *src++;
        }
    }
    else /* nand flash start */
    {
        nand_flash_read(0, (char *)dst, len);
    }
}

void clear_bss(void)
{
    extern int __bss_start__, __end__;
    volatile unsigned int *start = (unsigned int *)&__bss_start__;
    volatile unsigned int *end = (unsigned int *)&__end__;

    while(start < end)
    {
        *start = 0;
        start ++;
    }
}
