#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "my_error.h"

#define FILE_NAME "/dev/lcd"
#define LCD_X_RES (480)
#define LCD_Y_RES (272)

int main(int argc, char *argv[])
{
    int fd = 0;
    int ret = 0;
    unsigned char i = 0;

    fd = open(FILE_NAME, O_RDWR);
    if (fd < 0)
    {
        PRINT_ERR("%s open fail \n", FILE_NAME);
    }

    while(1)
    {
        usleep(1000 * 100);

#if 1
        for (i = 0; i < 0xFF; i++)
        {
            ret = write(fd, &i, LCD_X_RES * LCD_Y_RES * 2);
            printf("i = %d\n", i);
            usleep(1000 * 300);
        }
#endif
    }

    close(fd);

    return 0;
}

