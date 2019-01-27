#include "timer.h"
#include "s3c2440_soc.h"


int timer0_init(void)
{
    /* Time clock = PCLK / {prescaler value+1} / {divider value}
    Set Time clock = 50K,
    PCLK = 50M, set prescaler = 125, divider = 8
    */

    //1 set prescaler = 125
    TCFG0 &= ~(0xFF);
    TCFG0 |= 0x7D;

    //2 set divider = 1/8
    /*
    0000 = 1/2
    0001 = 1/4
    0010 = 1/8
    0011 = 1/16
    01xx = External TCLK0
    */
    TCFG1 &= ~(0xF);
    TCFG1 |= 0x2;

    //3 set TCNTBn and TCMPBn(16bit)
    TCNTB0 = 0x61A8; //25000
    //TCMPB0 not use

    //4 manual update TCNTBn and TCMPBn
    TCON |= (0x1 << 1);

    //5 auto update and start Timer
    TCON &= ~(0x1 << 1);
    TCON |= (0x1 | (0x1 << 3));

    return 0;
}
