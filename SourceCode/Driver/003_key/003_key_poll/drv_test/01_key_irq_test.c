#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>

#define KEY_IRQ_FILE_NAME "/dev/key_irqs"

int key_irq_test_start(void *data)
{
    int fd = 0;
    int ret = 0;

    int key_val;

    struct pollfd poll_fds;

    fd = open(KEY_IRQ_FILE_NAME, O_RDWR);
    if (fd < 0)
    {
        printf("open failed:%s\n", KEY_IRQ_FILE_NAME);
        return -1;
    }

    poll_fds.fd = fd;
    poll_fds.events = POLLIN;

    while(1)
    {
        ret = poll(&poll_fds, 1, 5000);

        if (0 == ret)
        {
            printf("time out\n");
        }
        else
        {
            ret = read(fd, &key_val, sizeof(key_val));
            if (ret < 0)
            {
                printf("raed fail:%d\n", ret);
                break;
            }

            printf("key_val:%#x\n", key_val);
        }
    }

    close(fd);

    return 0;
}

int key_irq_test_end(void *data)
{

    return 0;
}

