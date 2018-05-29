#include<stdio.h>
#include<sys/types.h>
#include<sys/msg.h>
#include<sys/ipc.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<stdlib.h>

#define TEXT_SIZE 1024
#define SERVER_TYPE 1
#define CLIENT_TYPE 2

struct msgbuf{
	long type;
	int msgtext;
};

int main(){
	int msqid,key;
	struct msgbuf server_send,server_recv;
	if((key = ftok("meg.test",1)) < 0 ){
	    perror("ftok error\n");
        exit(EXIT_FAILURE);
	}
    printf("get the server_key: %d\n",key);
    if((msqid = (msgget(key,0600|IPC_CREAT))) < 0){
        perror("msgget error!\n");
        exit(EXIT_FAILURE);
    }
    printf("the server msqid:%d\n",msqid);
    server_send.type = SERVER_TYPE;
    server_send.msgtext = getpid();
    printf("the pid of server is: %d\n",server_send.msgtext);

    if(msgsnd(msqid,&server_send,(sizeof(struct msgbuf) - sizeof(long)),0) < 0){
        perror("send error!\n");
        exit(EXIT_FAILURE);
    }
    printf("send success!\n");

    if(msgrcv(msqid,&server_recv,(sizeof(struct msgbuf) - sizeof(long)),CLIENT_TYPE,0) < 0){
        perror("receive error!\n");
        exit(EXIT_FAILURE);
    }   
    printf("get the message from the client: %d\n ",server_recv.msgtext);

    return 0;

}
