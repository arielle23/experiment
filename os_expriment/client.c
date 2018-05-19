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
	struct msgbuf client_recv,client_send;
    if((key = ftok("meg.test",1)) < 0 ){
        perror("ftok error\n");
        exit(EXIT_FAILURE);
    }
    printf("get the client key: %d\n",key);
    if((msqid = msgget(key,0)) < 0){
        perror("get msgid error!\n");
        exit(EXIT_FAILURE);
    }
    printf("client msqid:%d\n",msqid);

    client_send.type = CLIENT_TYPE;
    client_send.msgtext = getpid();
    printf("the pid of client: %d\n",client_send.msgtext);
    if(msgsnd(msqid,&client_send,(sizeof(struct msgbuf) - sizeof(long)),0) < 0){
        perror("send error!\n");
        exit(EXIT_FAILURE);                                                     
    }
    printf("send success!\n");

    if(msgrcv(msqid,&client_recv,(sizeof(struct msgbuf) - sizeof(long)),SERVER_TYPE,0) < 0){ 
        perror("receive message error!\n");
        exit(EXIT_FAILURE);
    }
    printf("get the message from the server : \nthe pid of server : %d\n",client_recv.msgtext);
    return 0;
    
}
