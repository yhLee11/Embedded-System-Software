//ku_sense.h
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <time.h>

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define SIMPLE_IOCTL_NUM 'z'
#define IOCTL_SWITCH _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM1, unsigned long*)

int start_sense_switch();
