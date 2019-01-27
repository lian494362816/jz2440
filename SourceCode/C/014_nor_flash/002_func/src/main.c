#include "uart.h"
#include "key.h"
#include "led.h"
#include "delay.h"
#include "my_printf.h"
#include "init.h"
#include "interrupt.h"
#include "s3c2440_soc.h"


//2018.9.1 测试bss 段的初始数据是否�?

int main(int argc, char *argv[])
{
    bank0_tacc_set(5);
    key_eint_init();
    led_init();
    timer0_init();

    nor_flash_test();

    while(1)
    {
        delay(500000);
    }

    return 0;
}
