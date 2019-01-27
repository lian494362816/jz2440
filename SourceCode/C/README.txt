
001_led_on
	点亮led
002_led_on
	修改bin文件点亮第2个led

003_led_c  
	用C语言点亮led

004_led_parma 
	可以传参数点亮led

005_leds
	001
		完善led程序
	002
		添加的头文件

006_key
	按键+Led程序
		
Makefile
	简单的Makefile 测试

007_clock
	配置时钟

008_uart
	简单的串口程序

009_uart_printf
	实现简单printf 函数，总的bin大小不能超过4K

010_nor_flash_tacc
	nor flash 时钟修改函数

011_dram
	sdram测试程序，注意烧录完后必须拔掉JTAG然后重启上电，不然即使不初始化sdram测试程序也会执行成功

012_relocate
	001	
		测试代码在程序中的位置，如bss data text rodata
	002 
		使用.lds重定位代码
	003
		使用宏+变量来写lds, 在start.s中使用lds的变量来实现，只需要修改lds就可以了
	004 
		测试bss段的默认值是否为0
	005 
		清除bss段的值，让其为0
	006 
		4字节拷贝/清除， 程序按4字节对齐
	007 
		改进lds 并将所有的代码拷贝进sdram
	008 
		使用C拷贝代码进sdram ，使用C清除bss段, 本来想使用ldr pc 来实现跳转，但是程序却无法回来

013_excption
    001_undef
        测试undef excption
    002_swi
        测试software interrut
    003_external_interrupt
        测试按键中断，发现EINT11，有时候松开没有触发中断
    004_timer_interrupt
        001_normal  
            定时器中断 
        002_func_pointer
            使用函数指针实现器中断

014_nor_flash
    001_test  
        nor flash 擦除 读 写 测试
    002_func
        nor flash 擦除 读 写 封装成函数
         
015_nand_flash
    nand flash 擦除 读 写， 程序可以从nand flash启动

016_lcd
    4.3寸 lcd 程序程序

017_touch_screen_adc
    001_get_x_y_position  
        获取触摸屏的x,y的数值, 程序流程感觉不理想

    002_calibrate
        对触摸屏进行校准，目前程序还有问题，校准效果不理想
    
018_cache_mmu
    001_enable_Icache 
        启动 instruction cache, 加快了刷lcd屏的速度

    002_enbale_mmu
        启动 instruction cache, data cache, mmuc, 刷lcd屏的速度提高数倍

019_simple_uboot
    最简单的uboot 启动流程
