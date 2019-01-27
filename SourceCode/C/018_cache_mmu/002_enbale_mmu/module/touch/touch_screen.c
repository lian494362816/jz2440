#include "s3c2440_soc.h"
#include "interrupt.h"
#include "my_error.h"

/*
dealy = (ADCDLY / X-Tal clock ) = ADCDLY / 12M
*/
#define ADC_DELAY_TIME (60000)

#define DETECT_UP_INT   (0x1 << 8)
#define DETECT_DONW_INT (0x0 << 8)

#define YM_ENABLE  (0x1 << 7)
#define YM_DISABLE (0x0 << 7)

#define YP_ENABLE  (0x0 << 6)
#define YP_DISABLE (0x1 << 6)

#define XM_ENABLE  (0x1 << 5)
#define XM_DISABLE (0x0 << 5)

#define XP_ENABLE  (0x0 << 4)
#define XP_DISABLE (0x1 << 4)

#define PULL_UP_ENABLE  (0x0 << 3)
#define PULL_UP_DISABLE (0x1 << 3)


/*
0 = Normal ADC conversion.
1 = Auto Sequential measurement of X-position, Y-position.
*/
#define AUTO_PST_ENABLE  (0x1 << 2)
#define AUTO_PST_DISABLE (0x0 << 2)

/*
00 = No operation mode
01 = X-position measurement
10 = Y-position measurement
11 = Waiting for Interrupt Mode
*/
#define XY_PST_NO_OPT (0x0 << 0)
#define XY_PST_WAIT_INT (0x3 << 0)

static int _g_ts_timer_enable = 0;
static int _g_x_ts = 0;
static int _g_y_ts = 0;
static int _g_status_ts = 0;
static char _g_xy_ts_valid = 0;

static void _ts_adc_wait_down(void)
{
    /*
    when automaticall measurement x, y postition
    the ADCTSC reg value will change, so need asigned value again
    */

    /* clear */
    ADCTSC &= ~(0x1FF);

    ADCTSC |= (DETECT_DONW_INT | YM_ENABLE | YP_DISABLE | XM_DISABLE | XP_DISABLE | PULL_UP_ENABLE | XY_PST_WAIT_INT | AUTO_PST_DISABLE);

    //PRINT_INFO("down ADCTSC:0x%x \n", ADCTSC);
}

static void _ts_adc_wait_up(void)
{
    /*
    when automaticall measurement x, y postition
    the ADCTSC reg value will change, so need asigned value again
    */

    /* clear */
    ADCTSC &= ~(0x1FF);

    ADCTSC |= (DETECT_UP_INT | YM_ENABLE | YP_DISABLE | XM_DISABLE | XP_DISABLE | PULL_UP_ENABLE | XY_PST_WAIT_INT | AUTO_PST_DISABLE);

    //PRINT_INFO("up ADCTSC:0x%x \n", ADCTSC);
}

/* aotomaticall measuremant x, y postition */
static void _ts_adc_auto(void)
{
    ADCTSC &= ~(0x1FF);
    /*
    after touch screen generate interrupt signal,
    wait for interrupt mode must be set to no operation mode
    */
    ADCTSC |= XY_PST_NO_OPT;

    /* set to auto measurement x,y position */
    ADCTSC |= AUTO_PST_ENABLE;

    /* start adc */
    ADCCON |= (0x1 << 0);
}

static int _ts_timer_get_status(void)
{
    return _g_ts_timer_enable;
}

static void _ts_timer_enable(void)
{
    //PRINT_INFO("time enable \n");
    _g_ts_timer_enable = 1;
}

static void _ts_timer_disable(void)
{
    //PRINT_INFO("time disable \n");
    _g_ts_timer_enable = 0;
}

static void _ts_adc_update_xy(int x, int y, int status)
{
    if (0 == _g_xy_ts_valid)
    {
        _g_x_ts = x;
        _g_y_ts = y;
        _g_status_ts = status;
        _g_xy_ts_valid = 1;
    }
}

int ts_adc_read_raw(int *px, int *py, int *pstatus)
{
    while(0 == _g_xy_ts_valid)
    {
        /* nothing */
    }

    *px = _g_x_ts;
    *py = _g_y_ts;
    *pstatus = _g_status_ts;
    _g_xy_ts_valid = 0;

    return 0;
}


static void _ts_timer_irq(void)
{
    if (0 == _ts_timer_get_status())
    {
        return;
    }

    /* donw status */
    if (!(ADCDAT0 & (0x1 << 15)))
    {
        _ts_adc_auto();
    }
    else
    {
        _ts_adc_update_xy(0, 0, 0);
    }
}


static void _ts_irq(int irq_num)
{
    /* ADCUPDN
    [1] Stylus Up Interrupt.
    0 = No stylus up status.
    1 = Stylus up interrupt occurred.

    [0] Stylus Down Interrupt.
    0 = No stylus down status.
    1 = Stylus down interrupt occurred.
    */

    /*Down INT */
    if (0x1 & ADCUPDN)
    {
        ADCUPDN &= ~(0x1 << 0);
        //PRINT_INFO("down INT \n");
        _ts_adc_auto();
        //_ts_adc_wait_up();
    }

    /*Up INT */
    if (0x2 & ADCUPDN)
    {
        ADCUPDN &= ~(0x1 << 1);

        //PRINT_INFO("up INT \n");
        _ts_timer_disable();
        _ts_adc_wait_down();
        _ts_adc_update_xy(0, 0, 0);
    }

}

static void _ts_adc_irq(int irq_num)
{
    static int count = 0;
    static int x = 0;
    static int y = 0;


    //PRINT_INFO("x:%d, y:%d \n", ADCDAT0 & 0x3FF, ADCDAT1 & 0x3FF);

    /* donw status */
    if (!(ADCDAT0 & (0x1 << 15)))
    {
        x += ADCDAT0 & 0x3FF;
        y += ADCDAT1 & 0x3FF;
        //printf("x:%d, y:%d \n", ADCDAT0 & 0x3FF, ADCDAT1 & 0x3FF);
        count ++;
        if (8 == count)
        {
            x >>= 3;
            y >>= 3;
            _ts_adc_update_xy(x, y, 1);

            x = 0;
            y = 0;
            count = 0;
            _ts_timer_enable();
        }
        else
        {
            _ts_adc_auto();
        }
    }
    else
    {
        x = 0;
        y = 0;
        count = 0;
        _ts_adc_update_xy(0, 0, 0);
        _ts_adc_wait_up();
    }

    _ts_adc_wait_up();

}

int ts_adc_int_handle(int irq_num)
{
    if ((0x1 << SUB_INT_TC) & SUBSRCPND)
    {
        SUBSRCPND |= (0x1 << SUB_INT_TC);
        _ts_irq(irq_num);
    }

    if ((0x1 << SUB_INT_ADC_S) & SUBSRCPND)
    {
        SUBSRCPND |= (0x1 << SUB_INT_ADC_S);
        _ts_adc_irq(irq_num);
    }
}

int ts_adc_reg_init(void)
{
    /* clear */
    ADCCON &= ~(0xFFFF);

    /*
    [14] A/D converter prescaler enable
    0 = Disable, 1 = Enable
    */
    ADCCON |= (0x1 << 14);

    /*
    [13:6] A/D converter prescaler value
    Data value: 0 ~ 255
    frequency = PCLK / (PRSCVL + 1)
    frequency = 50M / (49 + 1) = 1M
    Conversion time = 1/(1MHz / 5cycles) = 1/200KHz = 5 us
    Max frequency is 2.5M
    */
    ADCCON |= (49 << 6);

    /*
    [5:3] Analog input channel select
    000 = AIN 0
    001 = AIN 1
    010 = AIN 2
    011 = AIN 3
    100 = YM
    101 = YP
    110 = XM
    111 = XP
    */
    ADCCON |= (0x0 << 3);

    /*
    [2] Standby mode select
    0 = Normal operation mode
    1 = Standby mode
    */
    ADCCON &= ~(0x1 << 2);

    /*
    [1] A/D conversion start by read
    0 = Disable start by read operation
    1 = Enable start by read operation
    */
    ADCCON &= ~(0x1 << 1);

    /*
    A/D conversion starts by enable.
    If READ_START is enabled, this value is not valid.
    0 = No operation
    1 = A/D conversion starts and this bit is cleared after the startup.
    */
    //ADCCON |= (0x1 <<0);

    ADCDLY = ADC_DELAY_TIME;

    return 0;
}

int ts_adc_int_init(void)
{
    interrupt_reg(INT_ADC, ts_adc_int_handle);

    INTSUBMSK &= ~(0x1 << SUB_INT_TC);
    INTSUBMSK &= ~(0x1 << SUB_INT_ADC_S);

    return 0;
}

int touch_screen_init(void)
{
    /* init touch screen register */
    ts_adc_reg_init();

    /* init adc and touch screen interrupt */
    ts_adc_int_init();

    /* wait stylus down */
    _ts_adc_wait_down();

    timer0_func_register("touchscreen", _ts_timer_irq);

    PRINT_INFO("2 touch screen init down !\n");

    /* five-point calibration */
    ts_calibrate();

    return 0;
}
