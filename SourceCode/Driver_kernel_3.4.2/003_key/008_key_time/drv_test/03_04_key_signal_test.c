#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>


#define KEY_POLL_FILE_NAME "dev/key_signals"

static int fd = 0;

void sig_handler (int sig_num)
{
    int ret = 0;
    int key_val;

    ret = read(fd, &key_val, sizeof(key_val));
    if (ret < 0)
    {
        printf("raed fail:%d\n", ret);
        return;
    }

    printf("key_val:%#x\n", key_val);
}


int key_signal_test_start(void *data)
{
    int ret = 0;
    int fcntl_flag = 0;

    fd = open(KEY_POLL_FILE_NAME, O_RDWR);
    if (fd < 0)
    {
        printf("open failed:%s\n", KEY_POLL_FILE_NAME);
        return -1;
    }
    
    /* register SIGIO signal */
    signal(SIGIO, sig_handler);

    /* send pid to driver    */
    fcntl(fd, F_SETOWN, getpid());

    /* get control flag  */
    fcntl_flag = fcntl(fd, F_GETFL);

    /* send fasync signal to driver     */
    fcntl(fd, F_SETFL, fcntl_flag | FASYNC);

    while(1)
    {
        sleep(1000000);
    }

    close(fd);

    return 0;
}

int key_signal_test_end(void *data)
{
    return 0;
}

