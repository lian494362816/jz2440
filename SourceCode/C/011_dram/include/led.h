#ifndef __LED_H__
#define __LED_H__

int led_init(void);

//led_num [4,6]
int led_on(int led_num);

//led_num [4.6]
int led_off(int led_num);

#endif
