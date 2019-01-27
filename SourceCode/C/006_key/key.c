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
    int i = 0;
    int key_gpf_value = 0;
    int key_gpg_value = 0;

    led_init();
    key_init();

    while(1)
    {
        key_gpf_value = GPFDAT;
        key_gpg_value = GPGDAT;

        /* 1 key release */
        if (key_gpf_value & (1 << 0))
        {
            led_off(6);
        }
        else /* 0 key press */
        {
            led_on(6);
        }

        if (key_gpf_value & (1 << 2))
        {
            led_off(5);
        }
        else
        {
            led_on(5);
        }

        if(key_gpg_value & (1 <<3))
        {
            led_off(4);
        }
        else
        {
            led_on(4);
        }
    }


    return 0;
}
