#include "s3c2440_soc.h"
#include "uart.h"

int uart0_init(void)
{
    /* 1 set gpio to uart mode
        Tx GPH2 [5:4] b10
        Rx GPH3 [7:6] b10
    */
    GPHCON &= ~((0x3 << 4) | (0x3 << 6));
    GPHCON |= (0x2 << 4) | (0x2 << 6);

    /* 2 set pull up */
    GPHUP |= (0x1 << 2) | (0x1 << 3);

    /* 3 set formar 8N1 */
    ULCON0 |= 0x3;

    /* 4 set Soruce clock PCLK and poling mode*/
    UCON0 = 0x5;

    /* 5 set baud rate 115200
    PCLK = 50M
    UBRDIVn   = (int)( UART clock / ( buad rate x 16) ) –1
    UBRDIVn = (50M / (115200 x 16))-1 = 26.1267 -> 26
    */
    UBRDIV0 = 26;

	return 0;
}

int getchar(void)
{
    int chr = 0;

    while (!(UTRSTAT0 & 0x1))
    {
        //nothing
    }

    chr = URXH0;

    return chr;
}

int putchar(int c)
{
    while (!(UTRSTAT0 & 0x4))
    {
        //nothing
    }

    UTXH0 = (unsigned char )c;
}


int puts(const char *s)
{
    while(*s)
    {
        putchar(*s++);
    }
}

