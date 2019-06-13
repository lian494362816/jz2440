
核心文件
    platform
        dma.c 
        s3c24xx-i2s.c
    machine
        s3c2440_wm8976.c 
    codec
        wm8976.c 
        wm8976.h

arm-alsa.tar.gz
    使用交叉编译arm-linux-4.3.2 编译好的alsa库

linux-3.4.2_alsa.patch
    制作好的patch

test.c
    test source code

测试1 (使用alsa-utils)
        1 打patch
        2 make uImage -j4; 重新下载uImage
        3 make
        
        下面是主要的ko
        sound/soc/codecs/snd-soc-wm8976.ko
        sound/soc/samsung/snd-soc-s3c2440-wm8976.ko
        sound/soc/samsung/snd-soc-s3c24xx-i2s.ko
        sound/soc/samsung/snd-soc-s3c24xx.ko

        4 insmod ko
            insmod snd-soc-s3c24xx.ko
            insmod snd-soc-wm8976.ko
            insmod snd-soc-s3c24xx-i2s.ko
            insmod snd-soc-s3c2440-wm8976.ko

        5 解压 arm-alsa.tar.gz 
            将arm-alsa复制到 nfs_root/usr/local (必须是文件系统的/usr/local 下，否则alsa命令无法正常使用)
        6 播放
            aplay xxx.wav
        7 录音
            arecord -t wav -f S16_LE -c 2 -d 10 -r 8000 ./record.wav

测试2 (使用代码)
    1 make; 执行当前路径的Makefile, 将编译test.c 产生main
    2 执行./main
        对mic 说话，声音将会传到耳机中

    
    
    
    

