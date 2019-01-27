#include "excption.h"
#include "my_printf.h"

void print_excption(unsigned int cpsr_status, char *string)
{
    printf("%s\n", string);
    printf("cpsr:0x%x\n", cpsr_status);
}

void print_cpsr(unsigned int cpsr)
{
    printf("cpsr:0x%x\n", cpsr);
}

void print_hello(void)
{
    printf("\n");
    printf("hello 2440\n");
}

void print_swi(unsigned int *pswi_value)
{
    printf("swi value:%d\n", *pswi_value & 0xFFFFFF);
}
