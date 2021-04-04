obj-m := ch3_mod_201814121.o

KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	gcc -o app app.c
clean: 
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm app

