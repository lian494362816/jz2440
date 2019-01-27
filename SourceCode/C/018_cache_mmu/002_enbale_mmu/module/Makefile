OBJS:=start.o  main.o

CUR_PATH=$(shell pwd)

MODULES:= $(filter-out $(CUR_PATH), $(MODULES))

all:$(OBJS)
	$(foreach N,$(MODULES),make -C $(N);)

%.o:%.c
	$(CROSS_COMPILE)gcc $(CFLAGS) -c -o $@ $< -MD -MF .$@.d

%.o:%.s
	$(CROSS_COMPILE)gcc $(CFLAGS) -c -o $@ $<

%.o:%.S
	$(CROSS_COMPILE)gcc $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJS) .*.o.d
	$(foreach N,$(MODULES),make -C $(N) clean;)

.PHONY: clean all
