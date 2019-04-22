002_ts_input测试方法

1. 替换kernel, 使用kernel/uImage_no_lcd

2. 将ts_lib  全部拷贝根目录

3. 加载kernel/lcd_drv.ko 和002_ts_input/ts_drv.ko

4. 修改 /etc/ts.conf第1行
    # module_raw input
    改为：
    module_raw input(去掉#号和第一个空格)

5. 设置环境变量
    export TSLIB_TSDEVICE=/dev/input/event0 
    export TSLIB_CALIBFILE=/etc/pointercal
    export TSLIB_CONFFILE=/etc/ts.conf
    export TSLIB_PLUGINDIR=/lib/ts
    export TSLIB_CONSOLEDEVICE=none
    export TSLIB_FBDEVICE=/dev/fb0

6. 执行测试程序 
    执行/bin/ts_calibrate, 然后在触摸屏上进行校准
    执行/bin/ts_test, 在触摸屏上滑动，查看光标的跟踪情况
