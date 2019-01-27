#include "interrupt.h"
#include "my_printf.h"
#include "s3c2440_soc.h"

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

int interrupt_init(void)
{
    //enable eint0 eint2 eint11&eint19
    INTMSK &= ~( (0x1 << 0) | (0x1 << 2) | (0x1 << 5) );
    INTMSK &= ~(0x1 << 10);

    return 0;
}


/*
EINTPEND
EINT23  [23]
EINT22  [22]
EINT21  [21]
EINT20  [20]
EINT19  [19]
EINT18  [18]
EINT17  [17]
EINT16  [16]
EINT15  [15]
EINT14  [14]
EINT13  [13]
EINT12  [12]
EINT11  [11]
EINT10  [10]
EINT9   [9]
EINT8   [8]
EINT7   [7]
EINT6   [6]
EINT5   [5]
EINT4   [4]
Reserved  [3:0]
*/

static int key_eint_irq(int eint_num)
{
    int eint_pend = EINTPEND;
    int key_gpf_value = GPFDAT;
    int key_gpg_value = GPGDAT;

    if (0 == eint_num)
    {
        if (key_gpf_value & (1 << 0))
        {
            led_off(6);
        }
        else /* 0 key press */
        {
            led_on(6);
        }
    }
    else if (2 == eint_num)
    {
        if (key_gpf_value & (1 << 2))
        {
            led_off(5);
        }
        else
        {
            led_on(5);
        }
    }
    else if (5 == eint_num)
    {
        if ((0x1 << 11) == eint_pend)
        {
            if(key_gpg_value & (1 <<3))
            {
                led_off(4);
            }
            else
            {
                led_on(4);

            }
        }
        else if ((0x1 << 19) == eint_pend)
        {
            if(key_gpg_value & (1 << 11))
            {
                led_off(4);
                led_off(5);
                led_off(6);
            }
            else
            {
                led_on(4);
                led_on(5);
                led_on(6);
            }
        }

    }

    EINTPEND = eint_pend;

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

    if ( 0 == irq_num || 2 == irq_num || 5 == irq_num)
    {
        key_eint_irq(irq_num);
    }

    if (10 == irq_num)
    {
        printf("timer 0\n");
    }

    //clear irq,  INTOFFSET will auto clear after clear SRCPND and INTPND;
    SRCPND = (1 << irq_num);
    INTPND = (1 << irq_num);

    return 0;
}

