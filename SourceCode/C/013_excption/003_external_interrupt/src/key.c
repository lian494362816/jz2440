#include "s3c2440_soc.h"
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

    return 0;
}
