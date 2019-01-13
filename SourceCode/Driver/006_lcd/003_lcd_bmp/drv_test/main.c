#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include "my_error.h"
#include "bmp.h"

#define BMP_FILE_NAME "./002.bmp"

#define FB_FILE_NAME "/dev/lcd"
#define LCD_X_RES (480)
#define LCD_Y_RES (272)

struct bmp_color_t{
    unsigned char blue;
    unsigned char red;
    unsigned char greed;
}__attribute__((packed));

int bmp_read(char *file)
{
    int bmp_fd = 0;
    int ret = 0;
    bmp_file_head file_head;
    bmp_info_head  info_head;

    bmp_fd = open(file, O_RDWR);
    if (bmp_fd < 0)
    {
        PRINT_ERR("open fail%s \n", file);
        return -1;
    }

    ret = read(bmp_fd, &file_head, sizeof(bmp_file_head));
    if (ret < 0)
    {
        PRINT_ERR("read fail \n");
        close(bmp_fd);
        return -1;
    }

    PRINT_INFO("size:%d \n", file_head.size);
    PRINT_INFO("off_bits:%d \n", file_head.off_bits);
    PRINT_INFO("type:%c %c \n", file_head.type[0], file_head.type[1]);

    ret = read(bmp_fd, &info_head, sizeof(bmp_info_head));
    if (ret < 0)
    {
        PRINT_ERR("read fail \n");
        close(bmp_fd);
        return -1;
    }

    PRINT_INFO("size:%d \n", info_head.size);
    PRINT_INFO("width:%d \n", info_head.width);
    PRINT_INFO("height:%d \n", info_head.height);
    PRINT_INFO("planes:%d \n", info_head.planes);
    PRINT_INFO("bit_count :%d \n", info_head.bit_count);
    PRINT_INFO("compression:%d \n", info_head.compression);
    PRINT_INFO("size_image:%d \n ", info_head.size_image);
    PRINT_INFO("x:%d \n", info_head.x_pels_per_meter);
    PRINT_INFO("y:%d \n", info_head.y_pels_per_meter);
    PRINT_INFO("use:%d \n", info_head.clr_used);
    PRINT_INFO("improtant:%d \n", info_head.clr_important);

    close(bmp_fd);

    return 0;
}

int main(int argc, char *argv[])
{
    int fb_fd = 0;
    int bmp_fd = 0;
    bmp_file_head file_head;
    bmp_info_head  info_head;
    int ret = 0;
    int i = 0;
    int fb_size = 0;
    unsigned short *fb_buff = NULL;
    struct bmp_color_t bmp_color;
    int line_w = 0;
    int line_h = 0;
    int pos = 0;

    //bmp_read(BMP_FILE_NAME);

    fb_fd = open(FB_FILE_NAME, O_RDWR);
    if (fb_fd < 0)
    {
        PRINT_ERR("%s open fail \n", FB_FILE_NAME);
        return -1;
    }

    bmp_fd = open(BMP_FILE_NAME, O_RDWR);
    if (bmp_fd < 0)
    {
        PRINT_ERR("%s open fail \n", BMP_FILE_NAME);
        close(fb_fd);
        return -1;
    }

    fb_size = LCD_X_RES * LCD_Y_RES * 2;

    fb_buff = mmap(0, fb_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if ((void *) -1 == fb_buff)
    {
        PRINT_ERR("mmap fail \n");
        close(fb_fd);
        close(bmp_fd);
        return -1;
    }

    ret = read(bmp_fd, &file_head, sizeof(bmp_file_head));
    if (ret < 0)
    {
        PRINT_ERR("read fail \n");
        close(fb_fd);
        close(bmp_fd);
        return -1;
    }

    ret = read(bmp_fd, &info_head, sizeof(bmp_info_head));
    if (ret < 0)
    {
        PRINT_ERR("read fail \n");
        close(fb_fd);
        close(bmp_fd);;
        return -1;
    }

    if (info_head.width != LCD_X_RES || info_head.height != LCD_Y_RES)
    {
        PRINT_ERR("resolution not equal \n");
        PRINT_ERR("expect x:%d y:%d \n", LCD_X_RES, LCD_Y_RES);
        PRINT_ERR("really x:%d y:%d \n", info_head.width, info_head.height);
        close(fb_fd);
        close(bmp_fd);
        return -1;
    }

    lseek(bmp_fd, file_head.off_bits, SEEK_SET);

    while(1)
    {
        pos = ((info_head.height - line_h - 1) * info_head.width) + (line_w);
        ret = read(bmp_fd, &bmp_color, sizeof(bmp_color));
        if (ret <= 0)
        {
            //PRINT_INFO("ret:%d \n", ret);
            break;
        }
        if (i >= fb_size / 2)
        {
            break;
        }
        /* RGB 565 */
        fb_buff[pos] = ((bmp_color.red >> 3) << 11) | ((bmp_color.greed >> 2) << 5) | (bmp_color.blue >> 3);
        i++;
        line_w ++;
        if (info_head.width == line_w)
        {
            line_w = 0;
            line_h++;
        }
    }
    PRINT_INFO("i:%d \n", i);

    while(1)
    {
        usleep(1000 * 10);
    }

    munmap(fb_buff, fb_size);

    close(fb_fd);
    close(bmp_fd);

    return 0;
}

