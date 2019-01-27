#include "s3c2440_soc.h"
#include "init.h"


int bank0_tacc_set(int value)
{
    /* BANKCON0 &= ~(0x7 << 8); */
    /* BANKCON0 |= (value << 8); */
    BANKCON0 = value << 8;

    return 0;
}

