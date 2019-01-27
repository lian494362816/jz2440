#include "lcd_controller.h"
#include "s3c2440_soc.h"

void s3c2440_lcd_pin_init(void)
{
    /* VD[23:0]
        LCD_PWREN VCLK VFRAME VLINE VM VSYNC HSYNC
    */
    GPCCON = 0xAAAAAAAA;
    GPDCON = 0xAAAAAAAA;

    /* set LCD light pin to output*/
    /* clear */
    GPBCON &= ~(0x3 << 0);
    /* set to out put mode */
    GPBCON |= (0x1 << 0);

   /* LCD_PWRDN */
    GPGCON |= (0x3 << 8);
}

void s3c2440_lcd_init(lcd_param_t *p_lcd_param)
{
    int clkval = 0;
    int bppmode = 0;
    unsigned int addr = 0;

    s3c2440_lcd_pin_init();

    /* clear [17:0] */
    LCDCON1 &= ~(0x3FFFF);

    /*
      CLKVAL [17:8], TFT: VCLK = HCLK / [(CLKVAL+1) x 2]
      9 = 100 / [(CLKVAL+1) x 2]
      CLKVAL = 100 / 9 / 2 -1 = 4.5
      CLKVAL = 100 /  p_lcd_param->st_time.clk / 2 - 1;
    */
    clkval = 100 / p_lcd_param->st_time.clk / 2 - 1;
    printf("clkval :%d\n", clkval);
    LCDCON1 |= (clkval << 8);

    /* [7], usr for STN */

    /*
        [6:5] PNRMODE
        00 = 4-bit dual scan display mode (STN)
        01 = 4-bit single scan display mode (STN)
        10 = 8-bit single scan display mode (STN)
        11 = TFT LCD panel
    */
    LCDCON1 |= (0x3 << 5);

    /*
    [4:1] BPPMODE
    0000 = 1 bpp for STN, Monochrome mode
    0001 = 2 bpp for STN, 4-level gray mode
    0010 = 4 bpp for STN, 16-level gray mode
    0011 = 8 bpp for STN, color mode (256 color)
    0100 = packed 12 bpp for STN, color mode (4096 color)
    0101 = unpacked 12 bpp for STN, color mode (4096 color)
    0110 = 16 bpp for STN, color mode (4096 color)
    1000 = 1 bpp for TFT
    1001 = 2 bpp for TFT
    1010 = 4 bpp for TFT
    1011 = 8 bpp for TFT
    1100 = 16 bpp for TFT
    1101 = 24 bpp for TFT
    */
    if (p_lcd_param->bpp)
    bppmode = p_lcd_param->bpp == 8  ? 0xB :\
              p_lcd_param->bpp == 16 ? 0xC :\
              p_lcd_param->bpp == 24 ? 0xD : -1;

    printf("bppmode:%x\n", bppmode);
    if (-1 == bppmode)
    {
        printf("bpp mode error:%d\n", p_lcd_param->bpp);
        return;
    }
    LCDCON1 |= (bppmode << 1);

    /*
        [0]
        0 = Disable the video output and the LCD control signal.
        1 = Enable the video output and the LCD control signal.
    */
    //LCDCON1 |= 0x1;


    /* clear */
    LCDCON2 = 0;

    /* [31:24] VBPD */
    LCDCON2 |= ((p_lcd_param->st_time.vbpd  - 1) << 24);

    /* [23:14] LINEVAL */
    LCDCON2 |= ((p_lcd_param->height - 1 ) << 14);

    /* [13:6] VFPD */
    LCDCON2 |= ((p_lcd_param->st_time.vfpd -1) << 6);

    /* [5:0] VSPW */
    LCDCON2 |= ((p_lcd_param->st_time.vspw - 1) << 0);

    /*clear [25:0]*/
    LCDCON3 &= (0x3FFFFFF);

    /* [25:19] HBPD */
    LCDCON3 |= ((p_lcd_param->st_time.hbpd -1) << 19);

    /* [18:8] HOZVAL */
    LCDCON3 |= ((p_lcd_param->width - 1) << 8);

    /* [7:0] HFPD */
    LCDCON3 |= ((p_lcd_param->st_time.hfpd -1) << 0);


    /* clear [7:0] */
    LCDCON4 &= ~(0xFF);

    /* [7:0] HSPW */
    LCDCON4 |= ((p_lcd_param->st_time.hspw - 1) << 0);


    /* clear [12:0]*/
    LCDCON5 &= ~(0x1FFF);

    /* [12] BPP24BL , not use*/

    /* [11] 0 = 5:5:5:1 Format 1 = 5:6:5 Format */
    LCDCON5 |= (0x1 << 11);

    /* [10] INVVCLK */
    LCDCON5 |= (p_lcd_param->st_pin_polarity.vlck << 10);

    /* [9] INVVLINE */
    LCDCON5 |= (p_lcd_param->st_pin_polarity.vline << 9);

    /* [8] INVVFRAME */
    LCDCON5 |= (p_lcd_param->st_pin_polarity.vframe << 8);

    /* [7] INVVD */
    LCDCON5 |= (p_lcd_param->st_pin_polarity.vvd << 7);

    /* [6] INVVDEN */
    LCDCON5 |= (p_lcd_param->st_pin_polarity.vdec << 6);

    /* [5] INVPWREN */
    LCDCON5 |= (p_lcd_param->st_pin_polarity.vpwren << 5);

    /* [4] INVLEND */
    LCDCON5 |= (p_lcd_param->st_pin_polarity.vlend << 4);

    /* [3] LCD_PWREN output signal enable/disable */
    //LCDCON5 |= (0x1 << 3);

    /* [2] LEND output signal enable/disable
        not use
    */
    LCDCON5 &= ~(0x1 << 2);

    /*
            [1] BSWP, [0] HWSWP
        8  bpp (BSWP = 1, HWSWP = 0)
        16 bpp (BSWP = 0, HWSWP = 1)
        24 bpp (BSWP = 0, HWSWP = 0, BPP24BL = 0)
    */
    switch(p_lcd_param->bpp)
    {
        case 8:
            LCDCON5 |= (0x1 << 1);
            break;

        case 16:
            LCDCON5 |= (0x1 << 0);
            break;

        case 24:
            /* nothing */
            break;

        default:
            printf("bpp mode error:%d\n", p_lcd_param->bpp);
            break;
    }

    /*clear [29:0] */
    LCDSADDR1 &= ~(0x3fffffff);

    addr = p_lcd_param->frame_addr;
    /* [29:21] LCDBAK ->A[30:22] */
    /* [20:0] LCDBASEU->A[21:1] */
    addr &= ~(0x1 << 31);
    LCDSADDR1 = (addr >> 1);

    /* clear [20:0] */
    LCDSADDR2 &= ~(0x1FFFFF);

    if (24 == p_lcd_param->bpp)
    {
        /* in 24 bpp mode, the data size is 4 byte */
        addr = p_lcd_param->frame_addr + p_lcd_param->height * p_lcd_param->width * 4;
    }
    else
    {
        addr = p_lcd_param->frame_addr + p_lcd_param->height * p_lcd_param->width * p_lcd_param->bpp / 8;
    }
    LCDSADDR2 = (addr >> 1) & 0x1FFFFF;

}

void s3c2440_lcd_enable(void)
{
    /* [3] LCD_PWREN output signal enable/disable */
    LCDCON5 |= (0x1 << 3);

    /* LCD light On/Off */
    GPBDAT |= (0x1 << 0);

    /* Enable/Disable the video output and the LCD control signal*/
    LCDCON1 |= 0x1;
}

void s3c2440_lcd_disable(void)
{
    /* [3] LCD_PWREN output signal enable/disable */
    LCDCON5 &= ~(0x1 << 3);

    /* LCD light On/Off */
    GPBDAT &= ~(0x1 << 0);

    /* Enable/Disable the video output and the LCD control signal*/
    LCDCON1 &= ~0x1;
}

lcd_controller_t s3c2440_lcd_controller = {
    .name = "s3c2440",
    .init    = s3c2440_lcd_init,
    .enable  = s3c2440_lcd_enable,
    .disable = s3c2440_lcd_disable,
};


int s3c2440_lcd_add(void)
{
    return lcd_controller_register(&s3c2440_lcd_controller);
}

