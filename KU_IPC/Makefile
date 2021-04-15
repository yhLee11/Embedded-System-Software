obj-m := ku_ipc.o

KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	gcc -c app.c ku_ipc_lib.c
	gcc -o app app.o ku_ipc_lib.o
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm app
