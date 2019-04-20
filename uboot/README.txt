打patch
    cd  /home/black/jz2440/uboot/u-boot-1.1.6
    patch -p1 < ../u-boot-1.1.6_jz2440.patch

编译
    make 100ask24x0_config
    make
    
u-boot-2014.01.patch    
    uboot 2014 01 gcc4.3.2 对应的patch
    可以配置kernel 3.4.2

    编译
        make smdk2440_config
        make
