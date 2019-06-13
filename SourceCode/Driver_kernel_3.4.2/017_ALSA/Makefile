
CROSS_COMPILE=arm-linux-gcc
CC=gcc
LIB_PATH= -L./arm-alsa/lib
LIBS += -lasound
HEAD_PATH= -I./arm-alsa/include/


all:
	$(CROSS_COMPILE) test.c  $(LIB_PATH)  $(LIBS) $(HEAD_PATH) -o main
clean:
