#ifndef __LCD_CONTROLLER_H__
#define __LCD_CONTROLLER_H__

#include "lcd.h"

typedef struct lcd_controller {
    char *name;
    void (*init)(lcd_param_t *);
    void (*enable)(void);
    void (*disable)(void);
}lcd_controller_t;

int lcd_controller_register(lcd_controller_t *p_lcd_controller);
int lcd_controller_select(char *name);
int lcd_controller_init(lcd_param_t *p_lcd_param);
int lcd_controller_enable(void);
int lcd_controller_disable(void);
int lcd_controller_add(void);


#endif
