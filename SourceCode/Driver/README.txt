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
