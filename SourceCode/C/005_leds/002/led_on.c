#include "s3c2440_soc.h"

//#define  GPFCON   (*((volatile unsigned int *)0x56000050))
//#define  GPFDAT    (*((volatile unsigned int *)0x56000054))

void delay(int delay_time)
{
    while(delay_time --)
    {
        /* nothing */
    }
}

/* Output =b01
   D10 led1->GPF4 [9:8]
   D11 led2->GPF5 [11:10]
   D12 led4->GPF6 [13:12]
*/

int main(void)
{
    int i = 0;
    /* clear mode */
    GPFCON &= ~((3 << 8) | (3 << 10) | (3 << 12));
    /* set to output mode */
    GPFCON |= ((1 << 8) | (1 << 10) | (1 << 12));
    /* all led off */
    GPFDAT |= (7 << 4);

    while (1)
    {
        GPFDAT |= (7 << 4);
        GPFDAT &= ~(i << 4);
        i++;

        if (8 == i)
        {
            i = 0;
        }
        delay(100000);
    }

    return 0;
}
