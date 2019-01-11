#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include "my_error.h"

#define FILE_NAME "/dev/fb0"
#define LCD_X_RES (480)
#define LCD_Y_RES (272)

int main(int argc, char *argv[])
{
    int fd = 0;
    int ret = 0;
    int i = 0;
    int fb_size = 0;
    struct fb_var_screeninfo fb_var_info;
    unsigned short *fb_buff = NULL;

    fd = open(FILE_NAME, O_RDWR);
    if (fd < 0)
    {
        PRINT_ERR("%s open fail \n", FILE_NAME);
        return -1;
    }

    if(ioctl(fd, FBIOGET_VSCREENINFO, &fb_var_info))
    {
        PRINT_ERR("error\n");
        close(fd);
        return -1;
    }

    //PRINT_INFO("xres:%d \n", fb_var_info.xres);
    //PRINT_INFO("yres:%d \n", fb_var_info.yres);
    //PRINT_INFO("bit per pixel:%d \n", fb_var_info.bits_per_pixel);

    fb_size = fb_var_info.xres * fb_var_info.yres * fb_var_info.bits_per_pixel / 8;

#if 0
    fb_buff = (unsigned short *)malloc(fb_size);
    if (NULL == fb_buff)
    {
        PRINT_ERR("malloc fail \n");
        close(fd);
        return -1;
    }


    for (i = 0; i < fb_size / 2; i++)
    {
         fb_buff[i] = 0xFF << 5;
    }

    PRINT_INFO("buff ok\n");

    ret = write(fd, fb_buff, fb_size);
    if (ret != fb_size)
    {
        PRINT_ERR("write fail:%d\n", ret);
        close(fd);
        return -1;
    }

    PRINT_INFO("write done \n");

    while(1)
    {
        usleep(1000 * 100);
    }
#endif

#if 1
    fb_buff = mmap(0, fb_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if ((void *) -1 == fb_buff)
    {
        PRINT_ERR("mmap fail \n");
        close(fd);
    }

    for (i = 0; i < fb_size / 2; i++)
    {
        fb_buff[i] = (0x1F) << 11;
    }

    while(1)
    {
        usleep(1000 * 10);
    }

    munmap(fb_buff, fb_size);
#endif
    close(fd);

    return 0;
}

