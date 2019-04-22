#include <linux/module.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/genhd.h>
#include <linux/hdreg.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/wait.h>
#include <linux/blkdev.h>
#include <linux/blkpg.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/backing-dev.h>
#include <linux/bio.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/dma.h>
#include "my_error.h"

/*
    reference z2ram.c xd.c
*/

#define RAM_BLOCK_MODULE_NAME "ramblock"
#define RAM_BLOCK_MAJOR (0)
#define RAM_BLOCK_SIZE (1024 * 128)
#define RAM_BLOCK_SECTOR_SIZE (512)

static int ram_block_major = 0;
static struct gendisk *ram_block_disk = NULL;
static struct request_queue *ram_block_queue = NULL;


static DEFINE_SPINLOCK(ram_block_lock);
static unsigned char *ram_block_buffer = NULL;

static void ram_block_request(struct request_queue  *q)
{
    struct request *req;
    unsigned long offset = 0;
    unsigned long len = 0;
    int err = 0;

    req = blk_fetch_request(q);
    while (req)
    {
        offset = blk_rq_pos(req) << 9;
        len  = blk_rq_cur_bytes(req);

        if (len > RAM_BLOCK_SIZE)
        {
            PRINT_ERR("len to big:0x%x \n", len);
            len = RAM_BLOCK_SIZE;
        }

        if (rq_data_dir(req) == READ)
        {
            //PRINT_INFO("read,len:%ld, offst:%ld \n", len, offset);
            memcpy(req->buffer, ram_block_buffer + offset, len);
        }
        else
        {
            //PRINT_INFO("write,len:%ld, offset:%ld \n", len, offset);
            memcpy(ram_block_buffer + offset, req->buffer, len);
        }
        if (!__blk_end_request_cur(req, err))
        {
            req = blk_fetch_request(q);
        }
        else
        {
            //PRINT_WRN("__blk_end_request_cur fail \n");
        }
    }
}

/* Use for fdisk commond*/
static int ram_block_getgeo(struct block_device *bdev, struct hd_geometry *geo)
{
    /* size = heads * cylinders &* sectors * 512 */
    geo->heads = 1;
    geo->cylinders = 16;
    geo->sectors = RAM_BLOCK_SIZE / geo->heads / geo->cylinders / 512;

    return 0;
}


static struct block_device_operations ram_block_fops = {
    .owner	= THIS_MODULE,
    .getgeo = ram_block_getgeo,
};

static int ram_block_init(void)
{
    ram_block_major = register_blkdev(RAM_BLOCK_MAJOR, RAM_BLOCK_MODULE_NAME);
    if (ram_block_major < 0)
    {
        PRINT_ERR("reg blkdev fail \n");
        return ram_block_major;
    }

    ram_block_disk = alloc_disk(16);
    if (!ram_block_disk)
    {
        PRINT_ERR("alloc disk fail \n");
        goto err_free_blkdev;
    }

    ram_block_queue = blk_init_queue(ram_block_request, &ram_block_lock);
    if (!ram_block_queue)
    {
        PRINT_ERR("blk init queue fail \n");
        goto err_free_disk;
    }

    ram_block_disk->major = ram_block_major;
    ram_block_disk->first_minor = 0;
    ram_block_disk->fops = &ram_block_fops;
    ram_block_disk->queue = ram_block_queue;
    sprintf(ram_block_disk->disk_name, RAM_BLOCK_MODULE_NAME);
    set_capacity(ram_block_disk, RAM_BLOCK_SIZE / RAM_BLOCK_SECTOR_SIZE);

    /* alloc buffer*/
    ram_block_buffer = kzalloc(RAM_BLOCK_SIZE, GFP_KERNEL);
    if (!ram_block_buffer)
    {
        PRINT_ERR("kzalloc fail \n");
        goto free_blk_queue;
    }
    add_disk(ram_block_disk);
    PRINT_INFO("%s init ok \n", RAM_BLOCK_MODULE_NAME);

    return 0;

free_blk_queue:
    blk_cleanup_queue(ram_block_queue);

err_free_disk:
    put_disk(ram_block_disk);

err_free_blkdev:
    unregister_blkdev(ram_block_major, RAM_BLOCK_MODULE_NAME);

    return -1;
}

static void ram_block_exit(void)
{
    del_gendisk(ram_block_disk);
    blk_cleanup_queue(ram_block_queue);
    put_disk(ram_block_disk);
    unregister_blkdev(ram_block_major, RAM_BLOCK_MODULE_NAME);

    kfree(ram_block_buffer);

    PRINT_INFO("%s exit \n", RAM_BLOCK_MODULE_NAME);
}

module_init(ram_block_init);
module_exit(ram_block_exit);

MODULE_LICENSE("GPL");


