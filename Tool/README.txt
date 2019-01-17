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

zlib-1.2.11.tar.gz
    制作jffs2需要用到的压缩工具

    编译
        ./configure --shared --prefix=/usr
        make 
        sudo make install

mtd-utils-05.07.23.tar.bz2
    制作jffs2的工具

    编译
        cd mtd-utils-05.07.23/util
        make 
        sudo make instal

    命令
        mkfs.jffs2 -n -s 2048 -e 128KiB -d nfs_root -o nfs_root.jffs2
        -n:不要每个擦除块上都加清除标志位
        -s:页大小 单位是Byte
        -e:每个块大小
        -d:文件系统目录
        -o:输出文件名

    系统默认是yaffs2, 需要修改bootargs来修改不文件系统类型
        set bootargs noinitrd root=/dev/mtdblock3 init=/linuxrc console=ttySAC0,115200 rootfstype=jffs2

gdb-8.2.tar.xz
    gdb工具

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

arm-linux-gcc-3.4.5-glibc-2.3.6.tar.bz2
    jz2440 交叉编译器

tslib-1.4.tar.gz
    触摸屏的校验、测试库
    
gdb/bin
    使用arm-linux-gcc-3.4.5编译出来的交叉编译gdb 
    
tslib-1.4.tar.gz
    触摸屏的校验程序

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

