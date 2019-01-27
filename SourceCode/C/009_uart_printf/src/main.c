#include "uart.h"
#include "key.h"
#include "led.h"
#include "delay.h"
#include "my_printf.h"

int main(int argc, char *argv[])
{
    int chr = 0;
    uart0_init();

	printf("This is www.100ask.org   my_printf test\n\r") ;
	printf("test char           =%c,%c\n\r", 'A','a') ;

	printf("test decimal number =%d\n\r",    123456) ;
	printf("test decimal number =%d\n\r",    -123456) ;
	printf("test hex     number =0x%x\n\r",  0x55aa55aa) ;
	printf("test string         =%s\n\r",    "www.100ask.org") ;
	printf("num=%08d\n\r",   12345);
	printf("num=%8d\n\r",    12345);
	printf("num=0x%08x\n\r", 0x12345);
	printf("num=0x%8x\n\r",  0x12345);
	printf("num=0x%02x\n\r", 0x1);
	printf("num=0x%2x\n\r",  0x1);

	printf("num=%05d\n\r", 0x1);
	printf("num=%5d\n\r",  0x1);

    return 0;
}
