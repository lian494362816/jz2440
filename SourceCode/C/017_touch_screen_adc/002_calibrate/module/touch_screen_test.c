#include "touch_screen.h"
#include "my_error.h"

int touch_screen_test(void)
{
    int x = 0;
    int y = 0;
    int status = 0;

    touch_screen_init();

    while(1)
    {
        ts_read(&x, &y, &status);
        PRINT_INFO("x:%d, y:%d, status:%d \n", x, y, status);
    }
}
