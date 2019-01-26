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

