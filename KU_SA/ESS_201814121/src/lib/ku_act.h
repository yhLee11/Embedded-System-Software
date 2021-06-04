//ku_act.h
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+2
#define SIMPLE_IOCTL_NUM 'z'
#define IOCTL_SPEAKER _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM1, unsigned long*)

void start_act_speaker(int cnt);
