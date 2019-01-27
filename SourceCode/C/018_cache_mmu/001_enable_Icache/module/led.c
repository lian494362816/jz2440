#include "s3c2440_soc.h"
#include "led.h"

 /* Output =b01
 D10 led1->GPF4 [9:8]
 D11 led2->GPF5 [11:10]
 D12 led4->GPF6 [13:12]
*/
int led_init(void)
{
    /* clear mode */
    GPFCON &= ~((3 << 8) | (3 << 10) | (3 << 12));
    /* set to output mode */
    GPFCON |= ((1 << 8) | (1 << 10) | (1 << 12));
    /* all led off */
    GPFDAT |= (7 << 4);

    return 0;
}

int led_on(int led_num)
{
    if (4 == led_num)
    {
        GPFDAT &= ~(1 << 4);
    }
    else if (5 == led_num)
    {
        GPFDAT &= ~(1 << 5);
    }
    else if (6 == led_num)
    {
        GPFDAT &= ~(1 << 6);
    }
    else
    {
        return -1;
    }

    return 0;
}

int led_off(int led_num)
{
    if (4 == led_num)
    {
        GPFDAT |= 1 << 4;
    }
    else if (5 == led_num)
    {
        GPFDAT |= 1 << 5;
    }
    else if (6 == led_num)
    {
        GPFDAT |= 1 << 6;
    }
    else
    {
        return -1;
    }

    return 0;
}

