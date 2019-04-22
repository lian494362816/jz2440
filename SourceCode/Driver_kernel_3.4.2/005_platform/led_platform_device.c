#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ptrace.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>


static struct resource led_dev_resource [] = {
    [0] = {
        .flags = IORESOURCE_MEM,
        .start = 0x56000050,
        /* 1st byte 0x56000050 ~ 0x56000053
           2nd btye 0x56000054 ~ 0x56000057
        */
        .end = 0x56000050 + 8 - 1,
        .name = "gpfcon",
    },

    [1] = {
        .flags = IORESOURCE_IO,
        .start = 4,
        .end = 4,
        .name = "gpf4",
    },

    [2] = {
        .flags = IORESOURCE_IO,
        .start = 5,
        .end = 5,
        .name = "gpf5",
    },

    [3] = {
        .flags = IORESOURCE_IO,
        .start = 6,
        .end = 6,
        .name = "gpf6",
    },
};

static void led_dev_release(struct device * pdev)
{
    //nothing
}

static struct platform_device led_device = {
    .name = "my_led",
    .resource = led_dev_resource,
    .num_resources = ARRAY_SIZE(led_dev_resource),
    .dev = {
        .release = led_dev_release,
    },
};

static int led_device_init(void)
{
    return platform_device_register(&led_device);
}

static void led_device_exit(void)
{
    platform_device_unregister(&led_device);
}

module_init(led_device_init);
module_exit(led_device_exit);

MODULE_LICENSE("GPL");

