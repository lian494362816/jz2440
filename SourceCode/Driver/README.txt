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

