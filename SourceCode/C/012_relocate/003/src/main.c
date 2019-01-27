#include "uart.h"
//#include "key.h"
//#include "led.h"
#include "delay.h"
#include "my_printf.h"
#include "init.h"

//2018.8.30 使用变量来写lds
//002的lds使用数值来写，发现编译出来不一样， hex_tab 没有看见了

char _g_char1 = 'A';
char _g_char2 = 'a';
int a;
int b = 0;
const char _g_const_char = 'B';

int main(int argc, char *argv[])
{
    uart0_init();

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
