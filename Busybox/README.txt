编译
    1, 在Makefile 176行设置交叉编译器
        CROSS_COMPILE   ?= arm-linux-
        
    2, 执行make menuconfig

    3, 执行make

    4, 执行make CONFIG_PREFIX=/home/black/jz2440/nfs_root install
