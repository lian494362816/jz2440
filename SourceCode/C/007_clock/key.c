#include "s3c2440_soc.h"

void delay(int delay_time)
{
    while(delay_time --)
    {
        /* nothing */
    }
}

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

int main(void)
{
    int i = 4;

    led_init();

    while (1)
    {
        for (i = 4; i <= 6; i++)
        {
            led_on(i);
            delay(1 * 100000);
            led_off(i);
        }
    }

    return 0;
}
