#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "test.h"

struct test_opt{
    int (*start)(void *);
    int (*end)(void *);
    char *string;  /* description string */
};

struct test_opt test_opts[] = {
    {
        key_irq_test_start,
        key_irq_test_end,
        "key irq test",
    },

    {
        key_poll_test_start,
        key_poll_test_end,
        "key poll test",
    },

    {
        key_signal_test_start,
        key_signal_test_end,
        "key signal test",
    },

    {
        key_lock_test_start,
        key_lock_test_end,
        "key lock test"
    },

    {
        key_mutex_lock_test_start,
        key_mutex_lock_test_end,
        "key mutex lock test",
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
        if (num < 0 || num >= sizeof(test_opts) / sizeof(test_opts[0]))
        {
            printf("please input [%d]~[%d]\n", 0, (int) (sizeof(test_opts) / sizeof(test_opts[0])) -1 );
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

