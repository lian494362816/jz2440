#include "uart.h"
#include "key.h"
#include "led.h"
#include "delay.h"
#include "my_printf.h"
#include "init.h"
#include "interrupt.h"
#include "s3c2440_soc.h"
#include "my_error.h"
#include "lcd.h"
#include "timer.h"

int main(int argc, char *argv[])
{
    bank0_tacc_set(5);
    key_eint_init();
    led_init();
    timer0_init();

    lcd_test();
    while(1)
    {
        delay(500000);
    }

    return 0;
}
