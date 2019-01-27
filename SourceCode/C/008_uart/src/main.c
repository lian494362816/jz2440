#include "uart.h"
#include "key.h"
#include "led.h"
#include "delay.h"


int main(int argc, char *argv[])
{
    int chr = 0;
    uart0_init();

    while(1)
    {
        chr = getchar();

        if ('\r' == chr)
        {
            putchar('\n');
        }

        if ('\n' == chr)
        {
            putchar('\n');
        }

        putchar(chr);
    }

    return 0;
}
