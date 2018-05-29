#include <unistd.h>  
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFSIZE 1024

int main(){
    pid_t first_pid,second_pid;
    int fd[2];
    char buf[BUFSIZE];
    const char *message1 = "Child process 1 is sending a message!\n";
    const char *message2 = "Child process 2 is sending a message!\n";

    if(pipe(fd) < 0){
        perror("pipe\n");
        exit(errno);
    }
    if((first_pid = fork()) < 0 ){
        perror("fork\n");
        exit(errno);
    }
    if(first_pid == 0){
        printf("I'm the first child,pid:%d\nSending the first message...\n",getpid());
        close(fd[0]);
        write(fd[1],(void *)message1,strlen(message1));
        close(fd[1]);
        _exit(EXIT_SUCCESS);
    }
    if((second_pid = fork()) < 0 ){
        perror("fork\n");
        exit(errno);
    }
    if(second_pid == 0){
        printf("I'm the second child,pid:%d\nSending the second message...\n",getpid());
        close(fd[0]);
        write(fd[1],(void *)message2,strlen(message2));
        close(fd[1]);
        _exit(EXIT_SUCCESS);
    }
    else{
        int status;
        close(fd[1]);
        waitpid(first_pid, &status, 0);
        waitpid(second_pid,&status,0);
        printf("I'm the father\n");
        int len = read(fd[0], buf, sizeof(buf)); 
        write(STDOUT_FILENO,(void *)buf,len);
        close(fd[0]);
        printf("Moriturus te saluto\n");
        exit(EXIT_SUCCESS);
    }
    return 0;
}