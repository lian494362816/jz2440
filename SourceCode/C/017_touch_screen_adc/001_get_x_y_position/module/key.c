#include "s3c2440_soc.h"
#include "interrupt.h"
#include "key.h"

/*
Input =b00
EINT0   GPF0 [1:0]
EINT2   GPF2 [5:4]
EINT11 GPG3 [7:6]
*/
int key_init(void)
{
    GPFCON &= ~(0x3 << 0);
    GPFCON &= ~(0x3 << 4);
    GPGCON &= ~(0x3 << 6);

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

int key_eint_irq(int eint_num)
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
00->Input 01->Output 10->EINT
EINT 0  GPF0  GPFCON[1:0]
EINT 2  GPF2  GPFCON[5:4]
EINT 11 GPG3  GPGCON[7:6]
EINT 19 GPG11 GPGCON[23:22]

000->Low level
001->High Level
01x->Falling edge trigger
10x->Rising edge trigger
11x->Both edge trigger
EXIN 0  EXTINT0 [2:0]
EINT 2  EXTINT0 [10:8]
EINT 11 EXTINT1 [14:12]
EINT 19 EXTINT2 [14:12]
*/

int key_eint_init(void)
{
    //1, set to eint mode
    //clear
    GPFCON &= ~( (0x3 << 0) | (0x3 << 4) );
    GPGCON &= ~( (0x3 << 6) | (0x3 << 22) );
    //set to EINT
    GPFCON |= ( (0x2 << 0) | (0x2 << 4) );
    GPGCON |= ( (0x2 << 6) | (0x2 << 22) );

    //2, set trigger mode
    //set to both edge trigger 111
    EXTINT0 |= ( (0x7 << 0) | (0x7 << 8) );
    EXTINT1 |= (0x7 << 12);
    EXTINT2 |= (0x7 << 12);

    //3, enable eint11 eint19 interrupt, the eint0 ~ eint3 don't need set
    EINTMASK &= ~( (0x1 << 11) | (0x1 << 19) );

    interrupt_reg(EINT0, key_eint_irq);
    interrupt_reg(EINT2, key_eint_irq);
    interrupt_reg(EINT8_23, key_eint_irq);

    return 0;
}


