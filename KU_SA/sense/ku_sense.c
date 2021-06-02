//ku_sense.c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
//#include "sense_mod.h"
//#include "ku_sense.h"
#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define SIMPLE_IOCTL_NUM 'z'
#define IOCTL_PIR _IOWR(SIMPLE_IOCTL_NUM,IOCTL_NUM1, unsigned long*)
struct sen_buf{
	int sensor_num;
};
int main(void){
	
	int dev;

	dev=open("/dev/sense_mod_dev",O_RDWR);
	

		
	//sleep(100);

	struct sen_buf tmp;
	ioctl(dev, IOCTL_PIR,&tmp);
	printf("%d\n",tmp.sensor_num);
	
	sleep(3);

	ioctl(dev, IOCTL_PIR, &tmp);
	printf("%d\n",tmp.sensor_num);

	sleep(3);

	ioctl(dev, IOCTL_PIR, &tmp);
	printf("%d\n", tmp.sensor_num);
	close(dev);
	return 0;
}
