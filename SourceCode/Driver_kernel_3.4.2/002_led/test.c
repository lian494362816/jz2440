#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char *argv[])
{
    int fd = 0;
    int ret = 0;
    int val = 0;
    int c = 0;

    fd = open("/dev/led0", O_RDWR);
    if (fd < 0)
    {
        printf("open failed\n");
        return -1;
    }

    while(1)
    {
        printf("please input:\n");
        printf("[0] turn off led\n");
        printf("[1] turn on led\n");
        printf("[q] exit\n");

        c = getchar();
        if ('0' == c)
        {
            val = 0;
            write(fd, &val, sizeof(val));
        }
        else if ('1' == c)
        {
            val = 1;
            write(fd, &val, sizeof(val));
        }
        else if ('q' == c)
        {
            break;
        }
    }

    close(fd);

    return 0;
}
