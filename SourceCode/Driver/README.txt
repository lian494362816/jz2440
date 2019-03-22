001_test
    最简单的字符设备注册程序

002_led
    通过write函数来实现点灯 灭灯

003_key
    001_read_key  
        可以通过read函数读取按键状态
    002_key_irq  
        通过中断和等待队列实现按键的读取
    003_key_poll  
        使用poll机制来读取按键值
    004_key_signal
        上层注册signal， 底层通过fasyc给上层发信号
    005_key_lock  
        使用全局变量来当锁，文件允许同时打开1次
    006_key_lock_atomic
        使用原子变量来当锁，文件允许同时打开1次
    007_key_lock_mutex
        使用互斥锁，文件允许同时打开1次
    008_key_time
        使用定时器来实现按键消斗

004_input_key
    使用input子系统来获取按键的数值

    测试方法1:
        1)insmod ko后, 使用hexdump 命名dump /dev/event1的数值,然后按下按键

        [nfs2440 ~]#hexdump /dev/event1 
        数据标号   秒        毫秒   type code    value
        0000000 0544 0000 6fd8 000c 0001 0026 0001 0000
        0000010 0544 0000 6fe4 000c 0000 0000 0000 0000
        0000020 0544 0000 cf5c 000d 0001 0026 0000 0000
        0000030 0544 0000 cf64 000d 0000 0000 0000 0000

    测试方法2:
        cat /dev/tty1  然后按键按键，可以看到数据显示在LCD屏上

    测试方法3：
        exec 0</dev/tty1  然后按下按键，可以看到数据显示在串口上
        
005_platform 
    使用driver-bus-device 来控制led

006_lcd
    注:如果要测试，需要先替换kerne
    006_lcd/kernel_ko/uImage_no_lcd, 这个kernel 不会启动lcd

    001_lcd
        将lcd屏点亮，通过drv_test可以让屏幕呈现不同的颜色

    002_lcd_fb
        通过frame buff 将lcd屏点亮
        测试方法: 
            1)
                insmod cfbcopyarea.ko
                insmod cfbfillrect.ko
                insmod cfbimgblt.ko
                insmod lcd_drv.ko
            2)
                echo hello > /dev/tty1, lcd屏上显示hello表示成功
    003_lcd_bmp
        在lcd 屏上显示图片
        
007_touch_screen
    001_ts
        触摸程序

    002_ts_input
        通过input 子系统来获取触摸屏数据, 可以使用tslib来进行测试
    
008_usb
    测试时，需要将原来自带的USB鼠标驱动去掉
    注意:每个鼠标可能数据的排列不一样

    001_usb_mouse_simple  
        简单的获取usb鼠标的一些信息

    002_usb_mouse_key
        获取鼠标的按键数值并打印出来

009_ramblock
    使用kazlloc 申请一块buff来当做块设备来使用
    
    测试方法：
    方法一：
        1 加载ko 
            insmode ram_block_drv.ko
        2 格式化
            mkfs.vfat /dev/ramblock
        3 挂载
            mount /dev/ramblock /mnt
        4 写文件到/mnt里面，看是否可以正常保存
            cd /mnt
            echo 111111 > 1.txt
            echo 222222>  2.txt
        5 取消挂载
            mount /mnt
        6 把/dev/ramblock 的内容制作成.bin文件
            cat /dev/ramblock > /mnt/ramblock.bin
        7 挂载ramblock.bin 文件 
            mount -o loop /mnt/ramblock.bin > /tmp
        8 查看tmp 里面是否有1.txt 2.txt, 若有则表示程序ok


    方法二：
        1 加载ko 
            insmode ram_block_drv.ko
        2 格式化
            mkfs.vfat /dev/ramblock
        3 是fdisk 对 /dev/ramblock进行分区，分区成功后可以测试某个分区
        测试的方法按法一的步骤3~8

010_nand
    nand flash 驱动
    
    测试方法

    1. 去掉kernel 自带的nand flash 驱动, 并重烧kernel
        Device Drivers
            -> Memory Technology Device (MTD) support (MTD [=y])
                -> NAND Flash support for S3C2410/S3C2440 SoC (MTD_NAND_S3C2410 [=n]) 

    2. 加载nand_drv.ko
        insmod nand_drv.ko

    3. 查看 /dev 下是否有mtd相关的节点
        [root@2440 \w]#ls /dev/mtd* -l
        crw-rw----    1 root     root      90,   0 Jan  1 00:11 /dev/mtd0
        crw-rw----    1 root     root      90,   1 Jan  1 00:11 /dev/mtd0ro
        crw-rw----    1 root     root      90,   2 Jan  1 00:11 /dev/mtd1
        crw-rw----    1 root     root      90,   3 Jan  1 00:11 /dev/mtd1ro
        crw-rw----    1 root     root      90,   4 Jan  1 00:11 /dev/mtd2
        crw-rw----    1 root     root      90,   5 Jan  1 00:11 /dev/mtd2ro
        crw-rw----    1 root     root      90,   6 Jan  1 00:11 /dev/mtd3
        crw-rw----    1 root     root      90,   7 Jan  1 00:11 /dev/mtd3ro
        brw-rw----    1 root     root      31,   0 Jan  1 00:11 /dev/mtdblock0
        brw-rw----    1 root     root      31,   1 Jan  1 00:11 /dev/mtdblock1
        brw-rw----    1 root     root      31,   2 Jan  1 00:11 /dev/mtdblock2
        brw-rw----    1 root     root      31,   3 Jan  1 00:11 /dev/mtdblock3

    4. 使用flash_eraseall 擦除其中一个分区, 擦除后默认是yaffs2文件系统
        flash_eraseall /dev/mtd3

    5. 挂载
        mount -t yaffs2 /dev/mtdblock3  /tmp

011_nor
    nor flash 驱动

    测试方法

        注：必须选择nor flash驱动，否则cpu无法访问到nor flash

    1, 加载ko
        insmod nor_drv.ko

    2, 查看/dev 是否有mtd相关节点
        [root@2440 \w]#ls /dev/mtd* -l
        crw-rw----    1 root     root      90,   0 Jan  1 00:22 /dev/mtd0
        crw-rw----    1 root     root      90,   1 Jan  1 00:22 /dev/mtd0ro
        crw-rw----    1 root     root      90,   2 Jan  1 00:22 /dev/mtd1
        crw-rw----    1 root     root      90,   3 Jan  1 00:22 /dev/mtd1ro
        brw-rw----    1 root     root      31,   0 Jan  1 00:22 /dev/mtdblock0
        brw-rw----    1 root     root      31,   1 Jan  1 00:22 /dev/mtdblock1

    3, 使用flash_eraseall 格式化
        ./flash_eraseall -j /dev/mtd1  -j 表示jffs2格式

    5, 挂载
        mount -t jffs2 /dev/mtdblock1  /mnt

012_net
    001_vir_net 
        虚拟网卡驱动
    002_dm9k
        dm9000c 网卡驱动

        测试方法    
            1, 将dm9000c_drv.c 拷贝到drivers/net/
            2, 修改drivers/net/Makefile 
                obj-$(CONFIG_DM9000) += dm9dev9000c.o
                =>obj-$(CONFIG_DM9000) += dm9000c_drv.o
            3， 重新编译内核并烧录

013_iis_wm8976g
    发现无法写IIS的相关寄存器赋值， 目前还没找到原因

014_new_chr
    001
        使用新的方法来实现字符设备 

        测试方法
            1, 加载ko ii2_wm8976_drv.ko
            2, 运行测试程序./drv_test /dev/chr_devX, X 代表0,1,2,3,4...
            发现只有chr_dev0,chr_dev1 可以运行成功
    002
        使用相同的主设备号创建2个驱动

        测试方法
            1, 加载ko ii2_wm8976_drv.ko
            2, 运行测试程序./drv_test /dev/chr_devX, X 代表0,1,2,3,4...
            发现chr_dev0,chr_dev1 会打印“open new chr open"
            发现chr_dev2,chr_dev3 会打印“open 2 new chr open"

015_dma
    使用DMA 来搬移数据

    测试方法
        1, 加载ko dma_drv.ko
        2, 运行测试程序 
            ./drv_test /dev/dma1 dma; 使用DMA来搬移数据
            ./drv_test /dev/dma1 cpu; 使用CPU来搬移数据

        程序总共搬移50000次 x 1K bytes
        实验数据如下:
            1) 使用cpu:             cpu使用率100%左右， 消耗时间为10.2s 左右
            2) 使用dma unit  mode : cpu使用率45%左右，  消耗时间为9.1s 左右
            3) 使用dma burst mode : cpu使用率50%左右，  消耗时间为8.1s 左右
        
