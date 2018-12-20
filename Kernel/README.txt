
打补丁
    cd linux-2.6.22.6
    patch -p1 < ../linux-2.6.22.6_jz2440.patch
    cp ../4.3_LCD_mach-smdk2440.c arch/arm/mach-s3c2440/mach-smdk2440.c

编译
    cp config_ok .config
    make uImage


