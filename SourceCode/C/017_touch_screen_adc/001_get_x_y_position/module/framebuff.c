#include "lcd.h"
#include "common.h"
#include "my_error.h"

static int _g_width = 0;
static int _g_height = 0;
static int _g_bpp_mode = 0;
static unsigned int _g_frame_buff_addr = 0;

static unsigned short lcd_frame_buff_24bpp_to_16bpp(unsigned int color)
{
    int r = 0;
    int g = 0;
    int b = 0;

    /* 24bpp XXRRGGBB
       16bpp R[15:11] G[10:5] B[4:0]
                5bit     6bit  5bit
    */

    /* get R G B data */
    r = (color >> 16) & 0xFF;
    g = (color >> 8) & 0xFF;
    b = color & 0xFF;

    r = r >> 3;
    g = g >> 2;
    b = b >> 3;

    return  (r << 11) | (g << 5) | b;
}

int lcd_frame_buff_init(void)
{
    return lcd_get_param(&_g_width, &_g_height, &_g_bpp_mode, &_g_frame_buff_addr);
}

int lcd_frame_buff_put_pixel(int x, int y, unsigned int color)
{
    unsigned int addr = 0;
    unsigned char *p_8bpp = NULL;
    unsigned short *p_16bpp = NULL;
    unsigned int *p_24bpp = NULL;

    if (x >= _g_width)
    {
        PRINT_ERR("x too big, max:%d, x:%d\n", _g_width, x);
        return -1;
    }

    if (y >= _g_height)
    {
        PRINT_ERR("y too big, max:%d, y:%d\n", _g_height, y);
        return -1;
    }

    addr = _g_frame_buff_addr + ((_g_width * _g_bpp_mode / 8 ) * y) + (x * _g_bpp_mode / 8);
    switch(_g_bpp_mode)
    {
        case 8:
            p_8bpp = (unsigned char *)addr;
            *p_8bpp = color;
            break;
        case 16:
            (unsigned short *)p_16bpp = (unsigned short *)addr;
            *p_16bpp = lcd_frame_buff_24bpp_to_16bpp(color);
            break;
        case 24:
             addr = _g_frame_buff_addr + ((_g_width * 4 ) * y) + (x * 4);
             (unsigned int *)p_24bpp = (unsigned int *)addr;
            *p_24bpp = color;
            break;
        default:
            PRINT_ERR("bpp mode:%d\n", _g_bpp_mode);
            return -1;
            break;
    }

    return 0;
}
