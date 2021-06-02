obj-m:=ku_sense.o
KDIR:=~/linux-rpi/
ARM:=ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnueabi-
PWD:=$(shell pwd)
default:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) modules
	arm-linux-gnueabi-gcc ku_app.c -o ku_app
clean:	
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) clean
	rm ku_app
scp:
	scp $(obj-m:.o=.ko) mknod.sh ku_app pi@10.10.10.12:~/

