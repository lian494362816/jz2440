#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include "my_error.h"

#define FILE_NAME "/dev/lcd"
#define LCD_X_RES (480)
#define LCD_Y_RES (272)

#define RED   (0x1F << 11)
#define GREEN (0x3F << 5)
#define BLUE  (0x1F)

int main(int argc, char *argv[])
{
    int fd = 0;
    int ret = 0;
    int i = 0;
    int j = 0;
    int fb_size = 0;
    unsigned short *fb_buff = NULL;

    unsigned short color[] = {RED, GREEN, BLUE};

    fd = open(FILE_NAME, O_RDWR);
    if (fd < 0)
    {
        PRINT_ERR("%s open fail \n", FILE_NAME);
        return -1;
    }

    fb_size = LCD_X_RES * LCD_Y_RES * 2;

    /* use write */
#if 0
    fb_buff = (unsigned short *)malloc(fb_size);
    if (NULL == fb_buff)
    {
        PRINT_ERR("malloc fail \n");
        close(fd);
        return -1;
    }

    while(1)
    {
        for (i = 0; i < fb_size / 2; i++)
        {
             fb_buff[i] = color[j % 3];
        }

        ret = write(fd, (unsigned char *)fb_buff, fb_size);

        if (ret != fb_size)
        {
            PRINT_ERR("write fail:%d\n", ret);
            close(fd);
            return -1;
        }

        j++;

        usleep(1000 * 300);
    }
#endif


    /* use mmap */
#if 1
    fb_buff = (unsigned short *)mmap(0, fb_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if ((void *) -1 == fb_buff)
    {
        PRINT_ERR("mmap fail \n");
        close(fd);
    }

    while(1)
    {
        for (i = 0; i < fb_size / 2; i++)
        {
            fb_buff[i] = color[j % 3];
        }

        j++;

        usleep(1000 * 300);
    }
    munmap(fb_buff, fb_size);
#endif
    close(fd);

    return 0;
}

