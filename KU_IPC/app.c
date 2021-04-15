//app.c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include "ku_ipc_lib.h"
#include "ku_ipc.h"
struct msg_buf{
	long type;
	int msqid;
	int msgsz;
	int msgflg;
	char text[128];
};
int main(void){
	fd_open();
	struct msg_buf tmp;
	struct msg_buf b;
	b.type=1;
	b.msqid=3;
	b.msgsz=5;
	b.msgflg=KU_IPC_CREAT;
	strcpy(b.text,"abc");

	int ret;
	ret =ku_msgget(b.msqid,b.msgflg);
	printf("%d\n",ret);
 //	ret=ku_msgclose(3);
//	printf("%d\n",ret);
	ret=ku_msgsnd(b.msqid, &b, b.msgsz, 0);
	printf("msgsnd: %p\n",b);
//	ku_msgrcv(b.msqid, &tmp, b.msgsz,3, 0);



	fd_close();

}
