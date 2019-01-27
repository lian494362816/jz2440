#include "lcd_controller.h"

#define LCD_CONTROLLER_MAX (10)

static lcd_controller_t *_g_p_array_lcd_controller[LCD_CONTROLLER_MAX];
static lcd_controller_t *_g_p_lcd_controller_select;

int lcd_controller_register(lcd_controller_t *p_lcd_controller)
{
    int i = 0;

    for (i = 0; i < LCD_CONTROLLER_MAX; i++)
    {
        if (!_g_p_array_lcd_controller[i])
        {
            _g_p_array_lcd_controller[i] = p_lcd_controller;
            return 0;
        }
    }

    return -1;
}

int lcd_controller_select(char *name)
{
    int i = 0;

    for (i = 0; i < LCD_CONTROLLER_MAX; i++)
    {
        if (_g_p_array_lcd_controller[i] && !strcmp(_g_p_array_lcd_controller[i]->name, name))
        {
            _g_p_lcd_controller_select = _g_p_array_lcd_controller[i];
            return 0;
        }
    }

    return -1;
}

int lcd_controller_init(lcd_param_t *p_lcd_param)
{
    if (_g_p_lcd_controller_select)
    {
        _g_p_lcd_controller_select->init(p_lcd_param);
        return 0;
    }

    return -1;
}

int lcd_controller_enable(void)
{
    if (_g_p_lcd_controller_select)
    {
        _g_p_lcd_controller_select->enable();
        return 0;
    }

    return -1;
}

int lcd_controller_disable(void)
{
    if (_g_p_lcd_controller_select)
    {
        _g_p_lcd_controller_select->disable();
        return 0;
    }
    return -1;
}

int lcd_controller_add(void)
{
    return s3c2440_lcd_add();
}
