
srcs = $(wildcard *.c)
objs = $(patsubst %.c, %.o, $(srcs))
dep_files := $(patsubst %, .%.d, $(objs))
dep_files := $(wildcard $(dep_files))
CFLAGS = -Werror -Iinclude
TARGET:=test

$(TARGET):$(objs)
#	echo $(srcs)
#	echo $(objs)
	gcc -o $(TARGET) $^
#	echo "dep_files = $(dep_files)"

ifneq ($(dep_files), )
include $(dep_files)
endif

%.o:%.c
	gcc $(CFLAGS) -c -o $@ $<  -MD -MF .$@.d

clean:
	rm *.o  $(TARGET)

distclean:
	rm $(dep_files)

.PHONY:clean
