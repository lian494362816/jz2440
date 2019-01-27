#include "uart.h"
//#include "key.h"
//#include "led.h"
#include "delay.h"
#include "my_printf.h"
#include "init.h"

//2018.8.27 dram init test
// sdram use bank6, start address is 0x30000000

#define SIZE (1000)

int sdram_test(void)
{
    int i = 0;
    volatile unsigned char *p = (volatile unsigned char *) 0x30000000;

    for (i = 0; i < SIZE; i++)
    {
        p[i] = 0x55;
    }

    for (i = 0; i < SIZE; i++)
    {
        //printf("p[%d]= %d\n", i, p[i]);
        if (0x55 != p[i])
        {
            printf("p[%d]= %d\n", i, p[i]);
            return -1;
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int c = 0;
    uart0_init();
    bank6_sdram_init();

    if (0 == sdram_test())
    {
        printf("sdram test success\n");
    }
    else
    {
        printf("sdram test failed \n");
    }

    return 0;
}
