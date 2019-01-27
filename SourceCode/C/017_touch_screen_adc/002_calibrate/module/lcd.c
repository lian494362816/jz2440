#include "lcd.h"
#include "s3c2440_soc.h"


#define LCD_MAX (10)

#define EXEC(func, ret_val) ({\
        int _ret = 0;\
        _ret = func;\
        if (ret_val != _ret )\
        {\
            printf("%s\n", #func);\
            printf("fail:%d\n", _ret);\
            return _ret;\
        }\
    })


static lcd_param_t *_g_p_array_lcd_param[LCD_MAX];
static lcd_param_t *_g_p_lcd_param_select;


int lcd_register(lcd_param_t *p_lcd_param)
{
    int i = 0;

    for (i = 0; i < LCD_MAX; i++)
    {
        if (!_g_p_array_lcd_param[i])
        {
            _g_p_array_lcd_param[i] = p_lcd_param;
            return 0;
        }
    }

    return -1;
}

int lcd_select(char *name)
{
    int i = 0;

    for (i = 0; i < LCD_MAX; i++)
    {
        if (_g_p_array_lcd_param[i] && !strcmp(_g_p_array_lcd_param[i]->name, name))
        {
            _g_p_lcd_param_select = _g_p_array_lcd_param[i];
            return 0;
        }
    }

    return -1;
}

int lcd_init(void)
{
    EXEC(lcd_4_3_add(), 0);

    EXEC(lcd_controller_add(), 0);

    EXEC(lcd_select("lcd_4_3"), 0);

    EXEC(lcd_controller_select("s3c2440"), 0);

    EXEC(lcd_controller_init(_g_p_lcd_param_select), 0);

    return 0;
}

int lcd_enable(void)
{
    return lcd_controller_enable();
}

int lcd_disable(void)
{
    return lcd_controller_disable();
}

int lcd_get_param(int *width, int *height, int *bpp_mode, unsigned int *frame_addr)
{
    if (_g_p_lcd_param_select)
    {
        *width = _g_p_lcd_param_select->width;
        *height = _g_p_lcd_param_select->height;
        *bpp_mode = _g_p_lcd_param_select->bpp;
        *frame_addr = _g_p_lcd_param_select->frame_addr;
        return 0;
    }

    return -1;
}

void lcd_read_rgb(void)
{
    unsigned int red = 0;
    unsigned int green = 0;
    unsigned int blue = 0;

    red = (GPDDAT & (0xFF << 8)) >> 8;
    green = GPDDAT & 0xFF;
    blue = (GPCDAT & (0xFF << 8)) >> 8;

    printf("red:  0x%x\n", red);
    printf("green:0x%x\n", green);
    printf("blue: 0x%x\n", blue);
}

void lcd_status_read(void)
{
    printf("LCDCON1:0x%x\n", LCDCON1);
    printf("LCDCON2:0x%x\n", LCDCON2);
    printf("LCDCON3:0x%x\n", LCDCON3);
    printf("LCDCON4:0x%x\n", LCDCON4);
    printf("LCDCON5:0x%x\n", LCDCON5);
    printf("LCDSADDR1:0x%x\n", LCDSADDR1);
    printf("LCDSADDR2:0x%x\n", LCDSADDR2);
}
