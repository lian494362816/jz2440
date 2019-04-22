#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>  

#define DO_CPU (0)
#define DO_DMA (1)
#define MAX_COUNT (50000)

int main(int argc, char *argv[])
{
    int fd = 0;
    int i = 0;
    clock_t start;
    clock_t end;

    if (argc != 3)
    {
        printf("use:%s /dev/dmax <cpu|dma> \n", argv[0]);
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

    start = clock();
    if (0 == strcmp("cpu", argv[2]))
    {
        while(i < MAX_COUNT)
        {
            ioctl(fd, DO_CPU);
            i ++;
        }
    }
    else if (0 == strcmp("dma", argv[2]))
    {
        while(i < MAX_COUNT)
        {
            ioctl(fd, DO_DMA);
            i ++;
        }
    }
    else
    {
        printf("use:%s /dev/dmax <cpu|dma> \n", argv[0]);
    }
    end = clock();

    printf("time:%lfs \n", (double)(end -start)/CLOCKS_PER_SEC);

    close(fd);

    return 0;
}


