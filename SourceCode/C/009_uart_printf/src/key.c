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

