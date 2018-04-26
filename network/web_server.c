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
#define NAME_SIZE 20

char *newbag(char *);
void spilt(char spilt,char *s,char *name,char *type);

int main(){

    struct sockaddr_in server_sockaddr,client_sockaddr;
    char buffer_recv[MAX_BUF];
    char file_name[NAME_SIZE];
    char file_type[NAME_SIZE];
    char file[NAME_SIZE];

    char *html = "text/html\r\n\r\n";
    char *css = "text/css\r\n\r\n";
    char *gif = "image/gif\r\n\r\n";
    char *png = "image/png\r\n\r\n";
    char *jpeg = "image/jpeg\r\n\r\n";
    char *header200 = "HTTP/1.1 200 OK\r\nContent-Type: ";

    int sin_len;
    char *new_bag;
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
    while(1){
        if((client_fd = accept(sock_fd,(struct sockaddr *)&client_sockaddr,&sin_len)) == -1){
            fprintf(stderr,"accept failed\n");
            exit(EXIT_FAILURE);
        }
        if(recv(client_fd,buffer_recv,MAX_BUF,0) > 0){
            // printf("buffer_recv: %s\n", buffer_recv);
            sscanf(buffer_recv,"GET %s HTTP/1.1",file);
        }
        // printf("filename:%s\n",file);
        spilt('.',file,file_name,file_type);
        if(strcmp(file_type,"html") == 0)
            strcat(header200,html);
        else if(strcmp(file_type,"jpeg") == 0)
            strcat(header200,jpeg);
        else if(strcmp(file_type,"gif") == 0)
            strcat(header200,gif);
        else if(strcmp(file_type,"css") == 0)
            strcat(header200,css);
        else if(strcmp(file_type,"png") == 0)
            strcat(header200,png);
        send(client_fd,header200,strlen(header200),0);
        send(client_fd,new_bag = newbag(file),strlen(new_bag),0);
    }
    close(sock_fd);
    // if((client_fd = accept(sock_fd,(struct sockaddr *)&client_sockaddr,&sin_len)) == -1){
    //     fprintf(stderr,"accept failed\n");
    //     exit(EXIT_FAILURE);
    // }
    // send(client_fd,header200,strlen(header200),0);
    // send(client_fd,(new_bag = newbag(buffer)),strlen(new_bag),0);
    // close(sock_fd);
    return 0;
}

char *newbag(char *file){
    int c,i = 0;
    FILE *fd;
    char buffer[MAX_BUF];
    if(*file == '/' )
        fd = fopen("test.html","r");
    else if((fd = fopen(file,"r")) == NULL)
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

void spilt(char spilt,char *s,char *name,char *type){
    int found;
       if(*s == '\0'){
        strcpy(name,"test");
        strcpy(type,"html");
        return;
    }
    for(found = 0;*s;s++){
        if(*s == spilt)
            found = 1;
        else{
            if(found)
                *type++ = *s;
            else
                *name++ = *s;
        }
    }
    return;
}


