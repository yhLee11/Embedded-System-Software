obj-m:=sense_mod.o
KDIR:=~/linux-rpi/
ARM:=ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnueabi-
PWD:=$(shell pwd)
default:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) modules
	arm-linux-gnueabi-gcc ku_sense.c -o ku_sense
clean:	
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) clean
	rm ku_sense
scp:
	scp $(obj-m:.o=.ko) mknod.sh ku_sense pi@10.10.10.12:~/

