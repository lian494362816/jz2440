#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "my_error.h"

#define FILE_NAME "/dev/led_drv"

int main(int argc, char *argv[])
{
    int fd = 0;

    fd = open(FILE_NAME, O_RDWR);
    if (fd < 0)
    {
        PRINT_ERR("%s open fail \n", FILE_NAME);
    }

    while(1)
    {
        usleep(1000);
    }

    close(fd);

    return 0;
}

