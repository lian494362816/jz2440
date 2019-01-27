#include "lcd.h"
#include "font_8x16.h"
#include "common.h"
#include "framebuff.h"
#include "my_error.h"

#define FONT_WIDTH (8)
#define FONT_HEIGHT (16)

static int _g_width = 0;
static int _g_height = 0;
static int _g_bpp_mode = 0;
static unsigned int _g_frame_buff_addr = 0;

int font_init(void)
{
    return lcd_get_param(&_g_width, &_g_height, &_g_bpp_mode, &_g_frame_buff_addr);
}

int font_put_char(int x, int y, char c, int color)
{
    int i = 0;
    int j = 0;
    unsigned char *dot = NULL;
    int offset = 0;

    /* get character */
    dot = &fontdata_8x16[c * FONT_HEIGHT];

    /* put dot */
    for (i = y; i < y + FONT_HEIGHT; i++)
    {
        offset = FONT_WIDTH - 1;
        for (j = x; j < x + FONT_WIDTH; j++)
        {
            if ((*dot) & (0x1 << offset))
            {
                lcd_frame_buff_put_pixel(j, i, color);
            }
            offset --;
        }
        dot ++;
    }

    return 0;
}

int font_put_string(int x, int y, char *string, int color)
{
    int i = 0;
    int j = 0;

    while(string[i])
    {
        if ('\n' == string[i])
        {
            y += FONT_HEIGHT;
        }
        else if ('\r' == string[i])
        {
            x = 0;
        }
        else
        {
            /* go to next line */
            if (x + FONT_WIDTH >= _g_width)
            {
                y += FONT_HEIGHT;
                x = 0;
            }

            font_put_char(x, y, string[i], color);
            x += FONT_WIDTH;
        }
        i++;
    }

    return 0;
}
