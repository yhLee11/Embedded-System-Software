//ku_app.c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include "ku_sense.h"
#include "ku_act.h"

int main(void){
	int cnt=0;
	cnt=start_sense_switch();
	
	if(cnt){
		printf("pressed switch button");	
		start_act_speaker(cnt);
	}
	else{
		printf("no sensing data\n");
	}

	return 0;
}
