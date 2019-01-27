#include "uart.h"
#include "key.h"
#include "led.h"
#include "delay.h"
#include "my_printf.h"
#include "init.h"

int led_test(void)
{
    int i = 0;

    led_init();

    for (i = 4; i <= 6; i++)
    {
        led_on(i);
        delay(100000);
        led_off(i);
    }
}

int main(int argc, char *argv[])
{
    int c = 0;
    uart0_init();

    while(1)
    {
        printf("please input num[0~7]\n\r");
        c = getchar();
        putchar(c);

        if (c >= '0' && c <= '7')
        {
            bank0_tacc_set(c - '0');
            led_test();
        }
        else
        {
           printf("please input num[0~7]\n\r");
        }
    }

    return 0;
}
