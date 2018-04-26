#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>

#define SERVER_PORT 7023
#define MAX_BUF 1024
#define BACKLOG 20

char *header200 = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
char *newbag(char *);

int main(){
    struct sockaddr_in server_sockaddr,client_sockaddr;
    char buffer[MAX_BUF];
    int sin_len;
    char *new_bag;
    char recv[MAX_BUF];
    int sock_fd,client_fd;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_sockaddr.sin_port = htons(SERVER_PORT);
    bzero(&(server_sockaddr.sin_zero),8);
    if((sock_fd = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP)) == -1){
        fprintf(stderr,"socket failed\n");
        exit(EXIT_FAILURE);
    }
    if((bind(sock_fd,(struct sockaddr *)&server_sockaddr,sizeof(struct sockaddr_in)) == -1)){
        fprintf(stderr,"bind failed\n");
        exit(EXIT_FAILURE);
    }
    if((listen(sock_fd,BACKLOG)) == -1){
        fprintf(stderr,"listen failed\n");
        exit(EXIT_FAILURE);
    }
    printf("lisenting ...\n");
    if((client_fd = accept(sock_fd,(struct sockaddr *)&client_sockaddr,&sin_len)) == -1){
        fprintf(stderr,"accept failed\n");
        exit(EXIT_FAILURE);
    }
    send(client_fd,header200,strlen(header200),0);
    send(client_fd,(new_bag = newbag(buffer)),strlen(new_bag),0);
    close(sock_fd);
    return 0;
}

char *newbag(char buffer[MAX_BUF]){
    int c,i = 0;
    FILE *fd;
    if((fd = fopen("test.html","r")) == NULL)
    {
        perror("fopen");
        exit(errno);
    }
    while((c = getc(fd)) != EOF)
        buffer[i++] = (char)c;
    buffer[i] = '\0';
    // printf("buffer = %s\n",buffer);
    char *ret = (char *)malloc(sizeof(char) * i + 1);
    strcpy(ret,buffer);
    fclose(fd);
    return ret;
}