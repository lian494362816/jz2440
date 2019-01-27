#ifndef __UART_H__
#define __UART_H__

int uart0_init(void);

int getchar(void);

int putchar(int c);

int puts(const char *s);

#endif
