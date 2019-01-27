#include "interrupt.h"
#include "my_printf.h"
#include "s3c2440_soc.h"

static irq_func irq_func_array[IRQ_NUM_MAX];

/*
0 = Service available, 1 = Masked
INT_ADC      [31]
INT_RTC      [30]
INT_SPI1     [29]
INT_UART0    [28]
INT_IIC      [27]
INT_USBH     [26]
INT_USBD     [25]
INT_NFCON    [24]
INT_UART1    [23]
INT_SPI0     [22]
INT_SDI      [21]
INT_DMA3     [20]
INT_DMA2     [19]
INT_DMA1     [18]
INT_DMA0     [17]
INT_LCD      [16]
INT_UART2    [15]
INT_TIMER4   [14]
INT_TIMER3   [13]
INT_TIMER2   [12]
INT_TIMER1   [11]
INT_TIMER0   [10]
INT_WDT_AC97 [9]
INT_TICK     [8]
nBATT_FLT    [7]
INT_CAM      [6]
EINT8_23     [5]
EINT4_7      [4]
EINT3        [3]
EINT2        [2]
EINT1        [1]
EINT0        [0]
*/

int interrupt_reg(int      irq_num, irq_func fp)
{
    if (irq_num < 0 || irq_num >= IRQ_NUM_MAX)
    {
        printf("irq num error:%d\n", irq_num);
        return -1;
    }

    irq_func_array[irq_num] = fp;
    INTMSK &= ~(0x1 << irq_num);

    return 0;
}

/*
INTOFFSET&SRCPND (irq num)
INT_ADC    31  INT_UART2    15
INT_RTC    30  INT_TIMER4   14
INT_SPI1   29  INT_TIMER3   13
INT_UART0  28  INT_TIMER2   12
INT_IIC    27  INT_TIMER1   11
INT_USBH   26  INT_TIMER0   10
INT_USBD   25  INT_WDT_AC97 9
INT_NFCON  24  INT_TICK     8
INT_UART1  23  nBATT_FLT    7
INT_SPI0   22  INT_CAM      6
INT_SDI    21  EINT8_23     5
INT_DMA3   20  EINT4_7      4
INT_DMA2   19  EINT3        3
INT_DMA1   18  EINT2        2
INT_DMA0   17  EINT1        1
INT_LCD    16  EINT0        0
*/

int irq_handle_c(void)
{
    int irq_num = INTOFFSET;

    irq_func_array[irq_num](irq_num);

    //clear irq,  INTOFFSET will auto clear after clear SRCPND and INTPND;
    SRCPND = (1 << irq_num);
    INTPND = (1 << irq_num);

    return 0;
}

