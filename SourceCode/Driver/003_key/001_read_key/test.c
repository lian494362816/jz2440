#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
    int fd = 0;
    int ret = 0;
    int val = 0;
    int count = 0;
    /* [0] EINT0
       [1] EINT2
       [2] EINT11
       [3] EINT19
    */
    int key_val[4] = {0};

    fd = open("/dev/keys", O_RDWR);
    if (fd < 0)
    {
        printf("open failed:%s\n", "/dev/keys");
        return -1;
    }

    while(1)
    {
        ret = read(fd, key_val, sizeof(key_val));
        if (ret < 0)
        {
            printf("raed fail:%d\n", ret);
            break;
        }

        if (!key_val[0] || !key_val[1] || !key_val[2]|| !key_val[3])
        {
            count ++;
            printf("%d:%d %d %d %d\n", count, key_val[0], key_val[1], key_val[2], key_val[3]);
        }

        usleep(1000 * 10);
    }

    close(fd);

    return 0;
}
