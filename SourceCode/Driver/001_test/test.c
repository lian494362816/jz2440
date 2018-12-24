#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char *argv[])
{
    int fd = 0;
    int ret = 0;
    int val = 0;

    fd = open("/dev/test0", O_RDWR);
    if (fd < 0)
    {
        printf("open failed\n");
        return -1;
    }

    ret = write(fd, &val, sizeof(val));


    return 0;
}
