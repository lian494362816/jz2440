#include "tslib.h"
#include "my_error.h"

static double _g_Kx = 0.0;
static double _g_Ky = 0.0;

static int _g_Xe_ts = 0;
static int _g_Ye_ts = 0;

static int _g_Xe_lcd = 0;
static int _g_Ye_lcd = 0;
static char _g_xy_swap = 0;

/*
----------------------------
|                          |
|  +(A)              (B)+  |
|                          |
|                          |
|                          |
|            +(E)          |
|                          |
|                          |
|                          |
|  +(D)              (C)+  |
|                          |
----------------------------
*/

static int _ts_get_point_data(int x, int y, int *px_ts, int *py_ts, int *pstatus_ts)
{
    int x_ts = 0;
    int y_ts = 0;
    int status_ts = 0;

    /* draw "+" */
    draw_cross(x, y, 0xFFFFFF);

    /* wait stylus donw */
    do{
        ts_adc_read_raw(&x_ts, &y_ts, &status_ts);
    }while(0 == status_ts);

    /* read the last x y data */
    do{
        *px_ts = x_ts;
        *py_ts = y_ts;
        *pstatus_ts = status_ts;
        ts_adc_read_raw(&x_ts, &y_ts, &status_ts);
        //PRINT_INFO("x:%d, y:%d, status:%d \n", *px_ts, *py_ts, *pstatus_ts);
    }while(1 == status_ts);

    PRINT_WRN("x:%d, y:%d, status:%d \n", *px_ts, *py_ts, *pstatus_ts);

    draw_cross(x, y, 0);

    return 0;
}

int ts_is_xy_sawp(int a_ts_x, int a_ts_y, int c_ts_x, int c_ts_y)
{
    int x_diff = 0;
    int y_diff = 0;

    x_diff = c_ts_x - a_ts_x;
    y_diff = c_ts_y - a_ts_y;

    if (x_diff < 0)
    {
        x_diff = -x_diff;
    }

    if (y_diff < 0)
    {
        y_diff = -y_diff;
    }

    PRINT_ERR("x_diff:%d, y_diff:%d \n", x_diff, y_diff);

    if (x_diff > y_diff)
    {
        return 0;
    }
    else if (x_diff < y_diff)
    {
        return 1;
    }
    else
    {
        PRINT_ERR("x_diff is equal y_diff \n");
        return -1;
    }

}
static void ts_tsxy_to_lcdxy(int x_ts, int y_ts, int *x_lcd, int *y_lcd);

int ts_calibrate(void)
{
    int width = 0;
    int height = 0;
    int bpp_mode = 0;
    unsigned int frame_addr = 0;
    int a_ts_x = 0, a_ts_y = 0;
    int b_ts_x = 0, b_ts_y = 0;
    int c_ts_x = 0, c_ts_y = 0;
    int d_ts_x = 0, d_ts_y = 0;
    int e_ts_x = 0, e_ts_y = 0;
    int s1_ts = 0, s2_ts = 0;
    int d1_ts = 0, d2_ts = 0;
    int s1_lcd = 0;
    int d1_lcd = 0;
    int x_lcd = 0, y_lcd = 0;

    int ts_status = 0;

    lcd_get_param(&width, &height, &bpp_mode, &frame_addr);

    /* fill black color background */
    fill_fb(0, width, 0, height, 0x0);

#if 1
    /* A(50, 50) */
    _ts_get_point_data(50, 50, &a_ts_x, &a_ts_y, &ts_status);

    /* B(width - 50, 50) */
    _ts_get_point_data(width - 50, 50, &b_ts_x, &b_ts_y, &ts_status);

    /* C(width - 50, height - 50) */
    _ts_get_point_data(width - 50, height - 50, &c_ts_x, &c_ts_y, &ts_status);

    /* D(50, height - 50) */
    _ts_get_point_data(50, height - 50, &d_ts_x, &d_ts_y, &ts_status);

    /*E (width / 2, height / 2,) */
    _ts_get_point_data(width / 2, height / 2, &e_ts_x, &e_ts_y, &ts_status);
#else
    /* E (width / 2, height / 2,) */
    _ts_get_point_data(width / 2, height / 2, &e_ts_x, &e_ts_y, &ts_status);

    /* D(50, height - 50) */
    _ts_get_point_data(50, height - 50, &d_ts_x, &d_ts_y, &ts_status);

    /* C(width - 50, height - 50) */
    _ts_get_point_data(width - 50, height - 50, &c_ts_x, &c_ts_y, &ts_status);

    /* B(width - 50, 50) */
    _ts_get_point_data(width - 50, 50, &b_ts_x, &b_ts_y, &ts_status);

    /* A(50, 50) */
    _ts_get_point_data(50, 50, &a_ts_x, &a_ts_y, &ts_status);
#endif
    _g_xy_swap = ts_is_xy_sawp(a_ts_x,  a_ts_y,  c_ts_x,  c_ts_y);

    PRINT_INFO("width:%d, height:%d \n", width, height);
    if (_g_xy_swap)
    {
        swap_int(&a_ts_x, &a_ts_y);
        swap_int(&b_ts_x, &b_ts_y);
        swap_int(&c_ts_x, &c_ts_y);
        swap_int(&d_ts_x, &d_ts_y);
        swap_int(&e_ts_x, &e_ts_y);
        PRINT_INFO("swap x y \n");
    }

    s1_lcd = width - 50 - 50;
    d1_lcd = height -50 - 50;

    s1_ts = b_ts_x - a_ts_x;
    s2_ts = c_ts_x - d_ts_x;
    d1_ts = d_ts_y - a_ts_y;
    d2_ts = c_ts_y - b_ts_y;

    _g_Kx = ((double)(2 * s1_lcd)) / (s1_ts + s2_ts);
    _g_Ky = ((double)(2 * d1_lcd)) / (d1_ts + d2_ts);

    _g_Xe_ts = e_ts_x;
    _g_Ye_ts = e_ts_y;

    _g_Xe_lcd = width / 2;
    _g_Ye_lcd = height / 2;

    ts_tsxy_to_lcdxy(a_ts_x, a_ts_y, &x_lcd, &y_lcd);
    PRINT_INFO("x_lcd:%d, y_lcd:%d \n", x_lcd, y_lcd);

    ts_tsxy_to_lcdxy(b_ts_x, b_ts_y, &x_lcd, &y_lcd);
    PRINT_INFO("x_lcd:%d, y_lcd:%d \n", x_lcd, y_lcd);

    ts_tsxy_to_lcdxy(c_ts_x, c_ts_y, &x_lcd, &y_lcd);
    PRINT_INFO("x_lcd:%d, y_lcd:%d \n", x_lcd, y_lcd);

    ts_tsxy_to_lcdxy(d_ts_x, d_ts_y, &x_lcd, &y_lcd);
    PRINT_INFO("x_lcd:%d, y_lcd:%d \n", x_lcd, y_lcd);

    ts_tsxy_to_lcdxy(e_ts_x, e_ts_y, &x_lcd, &y_lcd);
    PRINT_INFO("x_lcd:%d, y_lcd:%d \n", x_lcd, y_lcd);

}

static void ts_tsxy_to_lcdxy(int x_ts, int y_ts, int *x_lcd, int *y_lcd)
{
    *x_lcd = (x_ts - _g_Xe_ts) * _g_Kx + _g_Xe_lcd;
    *y_lcd = (y_ts - _g_Ye_ts) * _g_Ky + _g_Ye_lcd;
}

int ts_read(int *px_lcd, int *py_lcd, int *pstatus)
{
    int x_ts = 0;
    int y_ts = 0;

    ts_adc_read_raw(&x_ts, &y_ts, pstatus);

    if (_g_xy_swap)
    {
        swap_int(&x_ts, &y_ts);
    }

    ts_tsxy_to_lcdxy(x_ts, y_ts, px_lcd, py_lcd);

    return 0;
}

