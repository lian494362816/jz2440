#include "timer.h"
#include "s3c2440_soc.h"
#include "interrupt.h"
#include "common.h"

#define TIME_OPT_MAX (32)

static time_opt_t _g_time0_func_array[TIME_OPT_MAX];

static int timer0_irq(int irq_num)
{
    int i = 0;

    for (i = 0; i < TIME_OPT_MAX; i++)
    {
        if (_g_time0_func_array[i].fp)
        {
            _g_time0_func_array[i].fp();
        }
    }

    return 0;
}

int timer0_func_register(char *name, time_func fp)
{
    int i = 0;

    for (i = 0; i < TIME_OPT_MAX; i++)
    {
        if (!_g_time0_func_array[i].fp)
        {
            _g_time0_func_array[i].name = name;
            _g_time0_func_array[i].fp = fp;
            return 0;
        }
    }

    return -1;
}

int timer0_func_unregister(char *name)
{
    int i = 0;

    for (i = 0; i < TIME_OPT_MAX; i++)
    {
        if (!strcmp(name, _g_time0_func_array[i].name))
        {
            _g_time0_func_array[i].name = NULL;
            _g_time0_func_array[i].fp = NULL;
            return 0;
        }
    }

    return -1;
}

int timer0_init(void)
{
    /* Time clock = PCLK / {prescaler value+1} / {divider value}
    Set Time clock = 50K,
    PCLK = 50M, set prescaler = 125, divider = 8
    */

    //1 set prescaler = 125
    TCFG0 &= ~(0xFF);
    TCFG0 |= 0x7D;

    //2 set divider = 1/8
    /*
    0000 = 1/2
    0001 = 1/4
    0010 = 1/8
    0011 = 1/16
    01xx = External TCLK0
    */
    TCFG1 &= ~(0xF);
    TCFG1 |= 0x2;

    //3 set TCNTBn and TCMPBn(16bit)
    TCNTB0 = 500;
    //TCMPB0 not use

    //4 manual update TCNTBn and TCMPBn
    TCON |= (0x1 << 1);

    //5 auto update and start Timer
    TCON &= ~(0x1 << 1);
    TCON |= (0x1 | (0x1 << 3));

    interrupt_reg(INT_TIMER0, timer0_irq);

    return 0;
}
