

=====ALSA=====
    alsa-lib-1.0.27.2.tar.bz2 
    alsa-utils-1.0.27.2.tar.bz2 
    ncurses-5.9.tar.gz
    编译方法
        切换到root
        root 设置交叉编译器 export PATH=$PATH:/opt/arm-linux-4.3.2/bin
        创建安装目录 mkdir -p /usr/local/arm-alsa注：这里名字为/usr/local/arm-alsa， 开发板的文件系统需要有相同的路径，否则alsa的工具无法正常使用
        lib
        解压 alsa-lib-1.0.27.2.tar.bz2
        ./configure --host=arm-linux --prefix=/usr/local/arm-alsa --disable-python
        make
        make install
        utils
        解压alsa-utils-1.0.27.2.tar.bz2
        ./configure --host=arm-linux --prefix=/usr/local/arm-alsa --with-alsa-inc-prefix=/usr/local/arm-alsa/include --with-alsa-prefix=/usr/local/arm-alsa/lib --disable-alsamixer --disable-xmlto
        make
        make install
        修改属性 chown black:black arm-alsa/ -R
        退出root exit
        拷贝整个arm-alsa 到文件系统中的相同位置
        cp arm-alsa/ ~/jz2440/nfs_root/usr/local/
        拷贝库到文件系统的lib
        cd arm-alsa/lib
        cp ./*so* -rfd ~/jz2440/nfs_root/lib

=====CrossCompile=====
    
    arm-linux-gcc-3.4.5-glibc-2.3.6.tar.bz2
    jz2440 交叉编译器, 针对kernel-2.8

    arm-linux-gcc-4.3.2.tar.bz2
    jz2440 交叉编译器, 针对kernel-3.5

Debug
    strace-4.5.16.tar.bz2
    strace_patch

    gdb-8.2.tar.xz
        gdb工具
        8.2版本的gdbsever 需要交叉编译器支持c++11, 因此暂时无法使用到gdbserver

        编译
            1)./configure --target=arm-linux --program-prefix=arm-linux- --prefix=/home/black/jz2440/Tool/gdb
                –target=arm-linux意思是说目标平台是运行于ARM体系结构的linux内核
                –program-prefix=arm-linux-是指生成的可执行文件的前缀，比如arm-linux-gdb，
                –prefix是指生成的可执行文件安装在哪个目录，这个目录需要根据实际情况作选择。如果该目录不存在，会自动创建，当然，权限足够的话

            2)make 
            3)make install 

            执行完make install后会在 /home/black/jz2440/Tool/gdb/bin 下生成这三个文件：
                arm-linux-gdb  这就是需要的gdb工具
                arm-linux-gdb-add-index  
                arm-linux-run

            生成的gdb工具会比较大，可以通过strip减小size
                strip arm-linux-gdb -o arm-linux-gdb-stripped

                -rwxr-xr-x 1 black root  65M 12月 27 13:01 arm-linux-gdb
                -rwxr-xr-x 1 black root 6.2M 12月 27 13:57 arm-linux-gdb-stripped
                -rwxr-xr-x 1 black root 3.2K 12月 27 13:01 arm-linux-gdb-add-index
                -rwxr-xr-x 1 black root 7.0M 12月 27 13:01 arm-linux-run

    termcap-1.3.1.tar.gz
        编译gdb-74._arm 依赖的库

        1) 解压 
            tar xf termcap-1.3.1.tar.gz

        2) 创建一个临时的目录
            mkdir tmp

        3) 修改编译工具
            cd termcap-1.3.1
            vim Makefile.in
            第23行 CC = @cc@ 
            改为==>CC = arm-linux-gcc

        4) 配置 
             ./configure --prefix=/home/black/jz2440/Tool/tmp/ --host=arm-linux

        5) 编译
            make 
            sudo make install

        6) 移动库
            cp /home/black/jz2440/Tool/tmp/lib/libtermcap.a /opt/gcc-3.4.5-glibc-2.3.6/arm-linux/lib

    gdb-7.4.tar.gz
        1 gdb 编译
        注:编译出来的工具只能在PC上运行
            1)./configure --target=arm-linux --program-prefix=arm-linux- --prefix=/home/black/jz2440/Tool/gdb-7.4_install/ --disable-werror
                –target=arm-linux意思是说目标平台是运行于ARM体系结构的linux内核
                –program-prefix=arm-linux-是指生成的可执行文件的前缀，比如arm-linux-gdb，
                –prefix是指生成的可执行文件安装在哪个目录，这个目录需要根据实际情况作选择。如果该目录不存在，会自动创建，当然，权限足够的话
                --disable-werror  忽略一些警告

            2)make 
            3)make install   可以能会报错，不过最终在gdb-7.4_install 成功产生了arm-linux-gdb

            执行完make install后会在 /home/black/jz2440/Tool/gdb-7.4_install/bin 下生成这三个文件：

            -rwxr-xr-x 1 root  root 22222632 4月  10 18:02 arm-linux-gdb
            -rwxr-xr-x 1 root  root 22222664 4月  10 18:02 arm-linux-gdbtui
            -rwxr-xr-x 1 root  root  4523912 4月  10 18:02 arm-linux-run

        2 gdbserver 编译
        注:编译出来的工具只能在arm平台上运行
            1)cd gdb-7.4/gdb/gdbserver
            2)./configure --host=arm-linux
            3)vim linux-arm-low.c 在里面开头开头添加  #include <linux/ptrace.h>, 不然程序会报错
            4)make   编译成功后在当前目录会生成gdbserver

            -rwxr-xr-x 1 black root 524434 4月  10 18:09 gdbserver


        3 gdb-7.4_arm
            可以直接在arm平台上运行的gdb, 都是使用的gdb-7.4.tar.gz

            依赖termcap这个库，所以需要先编译termcap-1.3.1.tar.gz 才行
            
            1) 配置
                ./configure --host=arm-linux --program-prefix=arm-linux- --prefix=/home/black/jz2440/Tool/gdb-7.4_arm/gdb-7.4_install --disable-werror --disable-tui
            
            2) 修改文件
                vim linux-arm-low.c 在里面开头开头添加  #include <linux/ptrace.h>, 不然程序会报错

            3) 编译
                make
                make install 
                编译完后在/home/black/jz2440/Tool/gdb-7.4_arm/gdb-7.4_install/bin 下有2个工具
                -rwxr-xr-x 1 black root 12725530 4月  10 21:49 arm-linux-gdb
                -rwxr-xr-x 1 black root  2586807 4月  10 21:49 arm-linux-run
    
    strace_patch
        编译 strace-4.5.16.tar.bz2 时用到的补丁

    strace-4.5.16.tar.bz2
        strace 工具源码
            
        编译步骤
            1,解压 
                tar -xf strace-4.5.16.tar.bz2

            2,打patch
                cd strace-4.5.16
                patch -p1 < ../strace_patch
                
            3, 配置
                ./configure --host=arm-linux CC=arm-linux-gcc

            4, 编译
                make

            编译完后会在当前目录下产生strace 可执行文件
            ls -lh strace
            -rwxr-xr-x 1 black root 377K 4月  10 14:04 strace

=====FileSystem=====
    mtd-utils-05.07.23.tar.bz2
        制作jffs2的工具

        编译
            cd mtd-utils-05.07.23/util
            make 
            sudo make install

        命令
            mkfs.jffs2 -n -s 2048 -e 128KiB -d nfs_root -o nfs_root.jffs2
            -n:不要每个擦除块上都加清除标志位
            -s:页大小 单位是Byte
            -e:每个块大小
            -d:文件系统目录
            -o:输出文件名

        系统默认是yaffs2, 需要修改bootargs来修改不文件系统类型
            set bootargs noinitrd root=/dev/mtdblock3 init=/linuxrc console=ttySAC0,115200 rootfstype=jffs2


    yaffs_source_util_larger_small_page_nand.tar.bz2
        可以制作2K,512B 页大小的yaffs文件系统

        制作yaffs2工具
            解压 yaffs_source_util_larger_small_page_nand.tar.bz2
                tar -xvf yaffs_source_util_larger_small_page_nand.tar.bz2

            编译
                cd Development_util_ok/yaffs2/utils
                make 
            拷贝到bin下，并加上只执行权限
                sudo cp mkyaffs2image /usr/local/bin
                sudo chmod a+x /usr/local/bin/mkyaffs2image


=====Other=====
    zlib-1.2.11.tar.gz
        制作jffs2需要用到的压缩工具

        编译
            ./configure --shared --prefix=/usr
            make 
            sudo make install
        
    tslib-1.4.tar.gz
        触摸屏的校验、测试库

            1 先安装编译需要的工具
                sudo apt-get install autoconf
                sudo apt-get install automake
                sudo apt-get install libtool

            2 编译
                解压 tar xzf tslib-1.4.tar.gz
                cd tslib
                ./autogen.sh 

                mkdir tmp;tmp用于存放生成的文件和库
                echo "ac_cv_func_malloc_0_nonnull=yes" >arm-linux.cache
                ./configure --host=arm-linux --cache-file=arm-linux.cache --prefix=$(pwd)/tmp
                make
                make install;会在tmp下生成bin,etc,include,lib

                安装：
                cd tmp
                cp * -rf /nfsroot;nfsroot 是网络文件系统的目录

            3 使用
                3.1
                先加载lcd 和 ts的驱动

                3.2
                修改 /etc/ts.conf第1行(去掉#号和第一个空格)：
                # module_raw input
                改为：
                module_raw input

                3.3
                export TSLIB_TSDEVICE=/dev/event0
                export TSLIB_CALIBFILE=/etc/pointercal
                export TSLIB_CONFFILE=/etc/ts.conf
                export TSLIB_PLUGINDIR=/lib/ts
                export TSLIB_CONSOLEDEVICE=none
                export TSLIB_FBDEVICE=/dev/fb0

                3.4
                执行/bin/ts_calibrate, 然后在触摸屏上进行校准
                执行/bin/ts_test, 在触摸屏上滑动，查看光标的跟踪情况

=====Wireless=====
    libnl-3.2.23.tar.gz
        编译iw工具依赖的库
        编译步骤
             1. 配置
                ./configure  --host=arm-linux --prefix=$PWD/tmp
             2. 编译
                make 
             3. 安装
                make install
             4. 拷贝库和头文件 
                4.1 cd tmp/include/libnl3
                4.2 sudo cp netlink -rf /opt/arm-linux-4.3.2/arm-none-linux-gnueabi/libc/usr/include/
                4.3 cd tmp
                4.4 sudo cp lib/* -rfd /opt/arm-linux-4.3.2/arm-none-linux-gnueabi/libc/armv4t/lib/
                4.5 将生成的库拷贝到文件系统的lib目录下  cp lib/* -rfd ~/jz2440/nfs_root/lib/

    iw-3.11.tar.bz2  
        搜索WiFi的工具
        编译步骤
            1 修改Makefile 
            2 make
            3 将生成的 iw 拷贝到文件系统的bin目录下 cp iw -rfd ~/jz2440/nfs_root/bin


    openssl-1.0.1d.tar.gz 
        编译 wpa_supplicant工具依赖的库 
        编译步骤
        1.  ./config shared no-asm --prefix=$PWD/tmp
        2.  修改Makefile:
                CC= arm-linux-gcc
                AR= arm-linux-ar $(ARFLAGS) r
                RANLIB= arm-linux-ranlib
                NM= arm-linux-nm
                MAKEDEPPROG= arm-linux-gcc
        3.  编译
            make
            make install
        4. 将生成的 openssl-1.0.1d/tmm/lib 库拷贝到文件系统的bin目录下 cp ./lib* -rfd ~/jz2440/nfs_root/bin

    wpa_supplicant-2.0.tar.gz
        连接Wifi的工具, open WEP WPA WPA2 的加密/认证
        1. 配置 .config
            cd wpa_supplicant-2.0/wpa_supplicant
            cp defconfig  .config 
            打开.config 在里面加上： CONFIG_LIBNL32=y
        2. 修改Makefile
            CC=arm-linux-gcc    

            CFLAGS += -I../../openssl-1.0.1d/tmp/include
            CFLAGS += -I../../libnl-3.2.23/tmp/include
            CFLAGS += -I../../libnl-3.2.23/tmp/include/libnl3/

            LDFLAGS += -L../../openssl-1.0.1d/tmp/lib
            LDFLAGS += -L../../libnl-3.2.23/tmp/lib
        3. 编译
            make 
            make DESTDIR=$PWD/tmp install
        4.  路径：wpa_supplicant/tmp/usr/local/sbin 下的文件就是需要的bin档

    dhcp-4.2.5-P1.tar.gz
        动态设置ip的工具
        1. 配置 
            ./configure --host=arm-linux ac_cv_file__dev_random=yes

        2. 修改lib/export/dns/Makefile.in 
            cd bin  
            tar -xf tar xzf dhcp-4.2.5-P1.tar.gz
            cd bind-9.8.4-P2
            vim lib/export/dns/Makefile.in (168行)
                gen: ${srcdir}/gen.c
                ${CC} ${ALL_CFLAGS} ${LDFLAGS} -o $@ ${srcdir}/gen.c ${LIBS}
                ==>
                gen: ${srcdir}/gen.c
                ${BUILD_CC} ${ALL_CFLAGS} ${LDFLAGS} -o $@ ${srcdir}/gen.c ${LIBS}
        3. 编译
            回到顶层目录 dhcp-4.2.5-P1
            make 
            make DESTDIR=$PWD/tmp install
        4. 拷贝
            dhcp-4.2.5-P1/tmp/usr/local/bin/* 拷贝到文件系统的bin
            dhcp-4.2.5-P1/tmp/usr/local/sbin/* 拷贝到文件系统的sbin
            dhcp-4.2.5-P1/tmp/usr/local/etc/* 拷贝到文件系统的etc
            dhcp-4.2.5-P1/client/scripts/linux  拷贝到文件系统的sbin
        5. 修改linux
            文件系统sbin下的linux 改名为dhclient-script
            vim dhclient-scripts
                #!/bin/bash
                ==>
                #!/bin/sh



