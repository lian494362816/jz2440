#include "uart.h"
//#include "key.h"
//#include "led.h"
#include "delay.h"
#include "my_printf.h"
#include "init.h"

//2018.8.29 .txt .data .bss .rodata
//测试代码烧录到nor 和 nand的结果
//因为printf函数的存在，使得_g_char的位置在0x30000010的位置
//如果把.data的第一个数据移到0x30000010 那么会从0开始打印
//如果把.datg的第一个数据移到0x30000000 那么也会从0开始打印，但是程序无法复位

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
