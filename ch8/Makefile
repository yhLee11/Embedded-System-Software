obj-m:=ch8_mod.o

KDIR:=~/linux-rpi/
ARM:= ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnueabi-
PWD:=$(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) modules
	arm-linux-gnueabi-gcc app.c -o app
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) clean
	rm app
scp:
	scp $(obj-m:.o=.ko) mknod.sh app pi@10.10.10.12:~/
