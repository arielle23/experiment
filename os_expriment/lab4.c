#include<stdio.h>
#include<sys/types.h>
#include<sys/msg.h>
#include<sys/ipc.h>
#include<string.h>
#include<errono.h>

#define TEXT_SIZE 1024

struct msgbuf{
	long type;
	char msgtext[TEXT_SIZE];
};

int main(){
	int msqid,key;
	struct msgbuf server_buf;
	if((key = ftok("meg.test",)) < 0 ){
		perror("ftok error");
	}


	
}
