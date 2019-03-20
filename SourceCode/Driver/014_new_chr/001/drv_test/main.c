#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    int fd = 0;

    if (argc < 2)
    {
        printf("use:  ./drv_test /dev/chr_devX \n");
        return -1;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        printf("open fail \n");
    }
    else
    {
        printf("open success \n");
    }

    close(fd);


    return 0;
}


