#include "lcd.h"
#include "my_error.h"
#include "font.h"

int lcd_test(void)
{
    int width = 0;
    int height = 0;
    int bpp_mode = 0;
    unsigned int frame_addr = 0;
    int i = 0;
    int j = 0;
    unsigned short *p_addr;
    unsigned int *p_addr2;


    lcd_init();

    lcd_enable();

    lcd_get_param(&width, &height, &bpp_mode, &frame_addr);
    PRINT_INFO("widht:%d, height:%d, bpp:%d, frame_addr:0x%x\n", width, height, bpp_mode, frame_addr);

#if 0
    if (16 == bpp_mode)
    {
        //red
        p_addr = (unsigned short *)frame_addr;
        for (i = 0; i < width; i++)
        {
            for (j = 0; j < height; j++)
            {
                *p_addr = 0xF800;
                p_addr ++;
            }
        }

        //green
        p_addr = (unsigned short *)frame_addr;
        for (i = 0; i < width; i++)
        {
            for (j = 0; j < height; j++)
            {
                *p_addr = 0x7E0;
                p_addr ++;
            }
        }

        //blue
        p_addr = (unsigned short *)frame_addr;
        for (i = 0; i < width; i++)
        {
            for (j = 0; j < height; j++)
            {
                *p_addr = 0x1F;
                p_addr ++;
            }
        }

        //black
        p_addr = (unsigned short *)frame_addr;
        for (i = 0; i < width; i++)
        {
            for (j = 0; j < height; j++)
            {
                *p_addr = 0;
                p_addr ++;
            }
        }

    }

    if (24 == bpp_mode)
    {
        //red
        p_addr2 = (unsigned int *)frame_addr;
        for (i = 0; i < width; i++)
        {
            for (j = 0; j < height; j++)
            {
                *p_addr2 = 0xFF0000;
                p_addr2 ++;
            }
        }

        //green
        p_addr2 = (unsigned int *)frame_addr;
        for (i = 0; i < width; i++)
        {
            for (j = 0; j < height; j++)
            {
                *p_addr2 = 0xFF00;
                p_addr2 ++;
            }
        }

        //blue
        p_addr2 = (unsigned int *)frame_addr;
        for (i = 0; i < width; i++)
        {
            for (j = 0; j < height; j++)
            {
                *p_addr2 = 0xFF;
                p_addr2 ++;
            }
        }

        //black
        p_addr2 = (unsigned int *)frame_addr;
        for (i = 0; i < width; i++)
        {
            for (j = 0; j < height; j++)
            {
                *p_addr2 = 0;
                p_addr2 ++;
            }
        }
    }
#endif
    lcd_frame_buff_init();

    fill_fb(0, width, 0, height, 0xFF0000);
    fill_fb(0, width, 0, height, 0xFF00);
    fill_fb(0, width, 0, height, 0xFF);

    fill_fb(0, width, 0, height, 0);

    draw_line(0, 0, width -1 , 0, 0xFF);
    draw_line(0, height -1, width -1, height-1, 0xFF);
    draw_line(0, 0, 0, height -1, 0xFF);
    draw_line(width -1, 0, width -1, height - 1, 0xFF);

    draw_circle(width / 2, height / 2, 50, 0xFF0000);

    font_init();

    font_put_char(10, 10, 'A', 0xFF);
    font_put_char(20, 10, 'B', 0xFF);
    font_put_char(30, 10, 'C', 0xFF);

    font_put_string(30, 30, "Font put char test !!!\n", 0xFF0000);
}
