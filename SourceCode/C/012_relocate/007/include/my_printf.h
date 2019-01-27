#ifndef __MY_PRINTF_H__
#define __MY_PRINTF_H__

#include "uart.h"
#define __out_putchar putchar

#define MAX_NUMBER_BYTES (128)

int printf(const char *fmt, ...);

#endif
