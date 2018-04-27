#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>

#define SERVER_PORT 7023
#define MAX_BUF 1024000
#define BACKLOG 20
#define NAME_SIZE 20

size_t newbag(char *file, void **block);
void spilt(char spilt,char *s,char *name,char *type);

int main(){
    struct sockaddr_in server_sockaddr,client_sockaddr;
    /*string to recognize the file*/
    char buffer_recv[MAX_BUF];
    char file_name[NAME_SIZE];
    char file_type[NAME_SIZE];
    char file_text[NAME_SIZE];
    /*the content type*/
    const char header200[] = "HTTP/1.1 200 OK\r\nContent-Type: ";
    const char html[] = "text/html\r\n\r\n";        
    const char css[] = "text/css\r\n\r\n";
    const char gif[] = "image/gif\r\n\r\n";
    const char png[] = "image/png\r\n\r\n";
    const char jpeg[] = "image/jpeg\r\n\r\n";
    const char jpg[] = "image/jpg\r\n\r\n";
    const char webm[] = "video/webm\r\n\r\n";

    int sin_len;
    void *new_bag;
    int sock_fd,client_fd;

    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_sockaddr.sin_port = htons(SERVER_PORT);
    bzero(&(server_sockaddr.sin_zero),8);

    /*initialized the socket,and listen.*/
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
    printf("lisenting port %d...\n", SERVER_PORT);

    while(1){
        char *header = (char *)malloc(sizeof(char) * MAX_BUF);
        strcpy(header, header200);
        if((client_fd = accept(sock_fd,(struct sockaddr *)&client_sockaddr,&sin_len)) == -1){
            fprintf(stderr,"accept failed\n");
            exit(EXIT_FAILURE);
        }
        if(recv(client_fd,buffer_recv,MAX_BUF,0) > 0){
            // printf("buffer_recv: %s\n", buffer_recv);
            sscanf(buffer_recv,"GET %s HTTP/1.1",file_text);
        }

        /*if there is no filename in http request*/
        if(strcmp(file_text,"/") == 0){
            strcpy(file_text, "/test.html");
            // strcat(header200,html);
            // printf("strcat\n");
        }
        // printf("filename:%s\n",file_text);
        /*concatnate http reponse*/
        {
            spilt('.', file_text, file_name, file_type);
            printf("\nFiletype: %s\n", file_type);
            if (strcmp(file_type, "html") == 0)
                strcat(header, html);
            else if (strcmp(file_type, "jpeg") == 0)
                strcat(header, jpeg);
            else if (strcmp(file_type, "gif") == 0)
                strcat(header, gif);
            else if (strcmp(file_type, "css") == 0)
                strcat(header, css);
            else if (strcmp(file_type, "png") == 0)
                strcat(header, png);
            else if (strcmp(file_type, "jpg") == 0)
                strcat(header,jpg);
            else if (strcmp(file_type, "webm") == 0)
                strcat(header,webm);
        }
        // printf("\nheader = %s\n",header);
        ssize_t send_byte = 0;
        send_byte = send(client_fd,header,strlen(header),0);
        // printf("header packet size %lu send %lu success\n", strlen(header), send_byte);
        printf("send the header\n");
        size_t bag_size = newbag(file_text, &new_bag);
        send_byte = send(client_fd,new_bag, bag_size,0);
        printf("packet size %lu send %lu success\n", bag_size, send_byte);
        free(new_bag);
        free(header);
        close(client_fd);
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

/*read the file and save it*/
size_t newbag(char *file, void **block){
    int c,i = 0;
    FILE *fd;
    printf("newbag %s\n",file);
    // if(strcmp(file,"/") == 0 ){
    //     fd = fopen("test.html","r");
    //     printf("open the file test.html\n");
    // }
    // else 
    if((fd = fopen(file + 1,"r")) == NULL)
    {
        perror("fopen");
        exit(errno);
    }
    *block = malloc(sizeof(char) * MAX_BUF);
    size_t fread_num = fread(*block, sizeof(char), MAX_BUF, fd);
    printf("fread %lu\n", fread_num );
    // printf("buffer = %s\n",buffer);
    // memcpy(ret, buffer, fread_num);
    fclose(fd);
    return fread_num;
}

/*spilt the filename by '.' */
void spilt(char spilt,char *s,char *name,char *type){
    int found;
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
    *type  = '\0';
    *name = '\0';
    return;
}


