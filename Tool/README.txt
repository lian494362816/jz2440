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

