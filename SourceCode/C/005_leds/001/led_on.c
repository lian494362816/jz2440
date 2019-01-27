
void delay(int delay_time)
{
    while(delay_time --)
    {
        /* nothing */
    }
}

/* Output =b01
   D10 led1->GPF4 [9:8]
   D11 led2->GPF5 [11:10]
   D12 led4->GPF6 [13:12]
*/

int main(void)
{
    volatile unsigned int *pGPFCON = (unsigned int *)0x56000050;
    volatile unsigned int *pGPFDAT = (unsigned int *)0x56000054;
    int i = 0;
    /* clear mode */
    *pGPFCON &= ~((3 << 8) | (3 << 10) | (3 << 12));
    /* set to output mode */
    *pGPFCON |= ((1 << 8) | (1 << 10) | (1 << 12));
    /* all led off */
    *pGPFDAT |= (7 << 4);

    while (1)
    {
        *pGPFDAT |= (7 << 4);
        *pGPFDAT &= ~(i << 4);
        i++;

        if (8 == i)
        {
            i = 0;
        }
        delay(100000);
    }

    return 0;
}
