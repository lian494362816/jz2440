#include "uart.h"
//#include "key.h"
//#include "led.h"
#include "delay.h"
#include "my_printf.h"
#include "init.h"

//2018.9.1 测试bss 段的初始数据是否是0

char _g_char1 = 'A';
char _g_char2 = 'a';
int _g_a;
int _g_b = 0;
const char _g_const_char = 'B';

int main(int argc, char *argv[])
{
    //uart0_init();

    printf("_g_a:%d\n", _g_a);
    printf("_g_b:%d\n", _g_b);

    while(1)
    {
        printf("%c", _g_char1);
        _g_char1 ++;

        printf("%c ", _g_char2);
        _g_char2 ++;
        delay(500000);
    }

    return 0;
}
