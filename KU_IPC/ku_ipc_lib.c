//ku_ipc_lib.c
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ku_ipc.h"
#include "ku_ipc_lib.h"

struct msgbuf{
	long type;
	int msqid;
	int msgsz;
	int msgflg;
	char text[128];
};
	
#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define IOCTL_NUM2 IOCTL_START_NUM+2
#define IOCTL_NUM3 IOCTL_START_NUM+3
#define IOCTL_NUM4 IOCTL_START_NUM+4
#define IOCTL_NUM5 IOCTL_START_NUM+5

#define SIMPLE_IOCTL_NUM 'z'
#define OPEN _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM1, unsigned long *)
#define READ _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM2, unsigned long *)
#define WRITE _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM3, unsigned long *)
#define CLOSE _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM4, unsigned long *)
#define CHECK _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM5, unsigned long *)

static int fd;

void fd_open(void){
	fd = open("/dev/ku_ipc_dev", O_RDWR);
	return 0;
}
void fd_close(void){
	close(fd);
}

int ku_msgget(int key, int msgflg){
	printf("ku_msgget()\n");

	int ret;

	switch(msgflg){
		case KU_IPC_CREAT:
			printf("ku_ipc_CREAT\n");
			ret = ioctl(fd, OPEN, key);
			return key;
			
		case KU_IPC_EXCL:
			printf("ku_ipc_EXCL\n");
			ret = ioctl(fd, OPEN, key);
			if(ret>0){
				printf("already exists queue: %d\n",ret);
				return -1;
			}
			return key; 

		default:
			printf("msgget error\n");
			return -1;	
	}		
	
}
int ku_msgclose(int msqid){
	printf("ku_msgclose()\n");
	
	int ret;

	ret = ioctl(fd, CLOSE, msqid);

	if(ret==-1){
		printf("msgclose fail\n");
		return ret;
	}
	else if(ret>9){
		printf("msgclose range error\n");
		return -1;
	}
	else{
		printf("msgclose success\n");
		return ret;
	}

}
int ku_msgsnd(int msqid, void *msgp, int msgsz, int msgflg){
	
	printf("ku_msgsnd()\n");

	int ret = 0;	

	ret = ioctl(fd, WRITE, &msgp);
	return ret;
	

}
int ku_msgrcv(int msqid, void *msgp, int msgsz, long msgtyp, int msgflg){
	printf("ku_msgrcv()\n");
	

	int ret=0;

	ret = ioctl(fd, READ, &msgp);
	return ret;
	
}
