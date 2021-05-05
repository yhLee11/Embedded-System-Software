#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
int main(void){
	int dev;
	dev=open("/dev/ch8_mod_dev",O_RDWR);
	sleep(100);
	close(dev);
}
