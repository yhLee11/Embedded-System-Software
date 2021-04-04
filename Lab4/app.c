#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include "ch3.h"

void main(void){

	int dev;
	dev = open("/dev/ch3_dev", O_RDWR);

	
	printf("read empty msg\n");
	struct msg_st empty;
	ioctl(dev, CH3_IOCTL_READ, &empty);
	printf("empty : %d, %s\n",empty.len,empty.str);	
	
	printf("ioctl write msg1\n");
	struct msg_st msg1;
	msg1.len = 1;
	strcpy(msg1.str, "a");
	ioctl(dev, CH3_IOCTL_WRITE, &msg1);

	printf("ioctl write msg2\n");
	struct msg_st msg2;
	msg2.len = 2;
	strcpy(msg2.str, "b");
	ioctl(dev, CH3_IOCTL_WRITE, &msg2);

	printf("ioctl read msg3\n");
	struct msg_st msg3;
	ioctl(dev, CH3_IOCTL_READ, &msg3);
       	printf("after read %d %s\n",msg3.len,msg3.str);

	printf("ioctl read msg4\n");
	struct msg_st msg4;
	ioctl(dev, CH3_IOCTL_READ, &msg4);	
	printf("after read %d %s\n",msg4.len,msg4.str);
	close(dev);
}

