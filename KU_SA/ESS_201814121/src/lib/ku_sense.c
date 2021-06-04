//ku_sense.c
#include "ku_sense.h"

int start_sense_switch(){
	
	int dev=-1;
	int count=0;

	printf("open sense_mod_dev(10seconds)\n");
	dev=open("/dev/sense_mod_dev",O_RDWR);

	sleep(10);
	
	count=ioctl(dev, IOCTL_SWITCH, NULL);
	printf("ku_sense: sensing count %d\n",count);
	
	close(dev);
	
	return count;
}

