#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mtd/map.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/types.h>
#include "my_error.h"


/*  reference drivers/mtd/maps/ts5500_flash.c */
#define NOR_MODULE_NAME "nor"

static const char probe_array [][16] = {"cfi_probe", "jedec_probe", "map_ram"};

static struct mtd_partition nor_part[] = {
    [0] = {
        .name = "tmp",
        .offset = 0,
        .size = 0x100000,
    },

    [1] = {
        .name = "root",
        .offset = MTDPART_OFS_APPEND,
        .size = MTDPART_SIZ_FULL,
    },
};

static struct mtd_info *nor_mtd = NULL;
static struct map_info nor_map = {
    .name = "nor flash",
    .size = 1024 * 1024 *2,
    .phys = 0x0,
     //2 *8 = 16bit
    .bankwidth = 2,
};

static int nor_init(void)
{
    int i = 0;

    nor_map.virt = ioremap(nor_map.phys, nor_map.size);
    if (!nor_map.virt)
    {
        PRINT_INFO("ioremap fail \n");
        return -1;
    }

    simple_map_init(&nor_map);

    for (i = 0; i < sizeof(probe_array)/sizeof(probe_array[0]); i++)
    {
        nor_mtd = do_map_probe(probe_array[i], &nor_map);
        if (nor_mtd)
        {
            PRINT_INFO("%s ", probe_array[i]);
            break;
        }
    }

    if (!nor_mtd)
    {
        PRINT_ERR("do_map_probe fail \n");
        goto err_unmap;
    }

    nor_mtd->owner = THIS_MODULE;
    if (add_mtd_partitions(nor_mtd, nor_part, ARRAY_SIZE(nor_part)))
    {
        PRINT_ERR("add mtd part fail \n");
        goto err_free_mtd;

    }

    PRINT_INFO("%s init ok \n", NOR_MODULE_NAME);

    return 0;

err_free_mtd:
    map_destroy(nor_mtd);

err_unmap:
    iounmap(nor_map.virt);

    return -1;
}

static void nor_exit(void)
{
    del_mtd_partitions(nor_mtd);

    map_destroy(nor_mtd);

    iounmap(nor_map.virt);

    PRINT_INFO("%s exit \n", NOR_MODULE_NAME);
}

module_init(nor_init);
module_exit(nor_exit);

MODULE_LICENSE("GPL");
