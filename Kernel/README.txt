
编译过程

    1.打补丁
        1.1 cd linux-2.6.22.6
        1.2 patch -p1 < ../linux-2.6.22.6_jz2440.patch
        1.3 cp ../4.3_LCD_mach-smdk2440.c arch/arm/mach-s3c2440/mach-smdk2440.c

    2.编译
        2.1 cp config_ok .config

        2.2 修改Makefile
            2.2.1 416 config %config: scripts_basic outputmakefile FORCE
                  =>%config: scripts_basic outputmakefile FORCE

            2.2.2 1449 / %/: prepare scripts FORCE
                  =>%/: prepare scripts FORCE

            2.2.3 896 (echo \#define UTS_RELEASE \"$(KERNELRELEASE)\";)
                 =>(echo \#define UTS_RELEASE \"$(KERNELVERSION)\";)

        2.3 make uImage -j4


linux-2.6.22.6_new.patch
    自己制作的patch, 使用此patch的编译步骤比较简单:
        1. 打补丁
            1.1 cd linux-2.6.22.6
            1.2 patch -p1 < ../linux-2.6.22.6_new.patch

        2. cp config_ok .config

        3. make uImage -j4

