//ku_act.c
#include "ku_act.h"

void start_act_speaker(int cnt){
	
	int dev;
	dev=open("/dev/act_mod_dev",O_RDWR);
	ioctl(dev, IOCTL_SPEAKER, cnt);

	close(dev);
}
