#ifndef __TIMER_H__
#define __TIMER_H__

typedef void (*time_func)(void);

typedef struct time_opt_s{
    char *name;
    time_func fp;
}time_opt_t;


int timer0_init(void);
int timer0_func_register(char *name, time_func fp);
int timer0_func_unregister(char *name);

#endif

