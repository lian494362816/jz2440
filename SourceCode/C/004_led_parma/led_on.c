
void delay(int delay_time)
{
    while(delay_time --)
    {
        /* nothing */
    }
}

int led_on(int led_num)
{
    unsigned int *pGPF4CON = (unsigned int *)0x56000050;
    unsigned int *pGPFDAT = (unsigned int *)0x56000054;

    if (4 == led_num)
    {
        *pGPF4CON = 0x100;
    }
    else if (5 == led_num)
    {
        *pGPF4CON = 0x400;
    }

    *pGPFDAT = 0x0;

    return 0;
}

