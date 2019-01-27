#include "uart.h"
//#include "key.h"
//#include "led.h"
#include "delay.h"
#include "my_printf.h"
#include "init.h"

//2018.8.29 .txt .data .bss .rodata 
//测试代码烧录到nor 和 nand的结果

char _g_char = 'A';
int a;
int b = 0;
const char _g_const_char = 'B';

int main(int argc, char *argv[])
{
    uart0_init();

	while(1)
	{
		printf("%c ", _g_char);
		_g_char ++;
		delay(500000);
	}

    return 0;
}
