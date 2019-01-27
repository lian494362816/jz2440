#include "uart.h"
#include "key.h"
#include "led.h"
#include "delay.h"
#include "my_printf.h"
#include "init.h"
#include "interrupt.h"
#include "s3c2440_soc.h"


//2018.9.1 测试bss 段的初始数据是否是0

int main(int argc, char *argv[])
{
    //uart0_init();

    interrupt_init();
    key_eint_init();
    led_init();

    while(1)
    {
        delay(500000);
    }

    return 0;
}
