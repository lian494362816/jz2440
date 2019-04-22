#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

struct test_opt{
    int (*start)(void *);
    int (*end)(void *);
    char *string;  /* description string */
};

int key_irq_test_start(void *data)
{
    int fd = 0;
    int ret = 0;

    int key_val;

    fd = open("/dev/key_irqs", O_RDWR);
    if (fd <= 0)
    {
        printf("open failed:%s\n", "/dev/key_irqs");
        return -1;
    }

    while(1)
    {
        ret = read(fd, &key_val, sizeof(key_val));
        if (ret < 0)
        {
            printf("raed fail:%d\n", ret);
            break;
        }

        printf("key_val:%#x\n", key_val);

        if (0x41 == key_val)
        {
            break;
        }
    }

    close(fd);

    return 0;
}

int key_irq_test_end(void *data)
{

    return 0;
}

struct test_opt test_opts[] = {
    {
        key_irq_test_start,
        key_irq_test_end,
        "key irq test"
    },
};

int main(int argc, char *argv[])
{
    int i = 0;
    int num = 0;

    while(1)
    {
        printf("=====test menu =====\n");
        for (i = 0; i < sizeof(test_opts) / sizeof(test_opts[0]); i++)
        {
            printf("%d.%s\n", i, test_opts[i].string);
        }
        printf("[q]-Quit\n");
        printf("please input a num:");
        num = getchar();

        if ('q' == num)
        {
            return 0;
        }

        num = num - '0';
        if (num < 0 || num > sizeof(test_opts) / sizeof(test_opts[0]))
        {
            printf("please input [%d]~[%d]\n", 0, (int) (sizeof(test_opts) / sizeof(test_opts[0])) );
            continue;
        }
        else
        {
            test_opts[num].start(NULL);
            test_opts[num].end(NULL);
        }
    }


    return 0;
}
