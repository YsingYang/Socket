#ifndef CLIENT.H

#define  CLIENT.H
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<signal.h>
#include<wait.h>
#include<sys/uio.h>
#include<string.h>
#include<errno.h>


void clientProcess(int fd);

void client(){
    int sockfd;
    sockaddr_in client_addr;
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(6666);
    ///inet_pton(sockfd,"127.0.0.1",&client_addr.sin_addr);
    client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(sockfd,(sockaddr*)&client_addr,sizeof(client_addr))<0){
        printf("connect  error \n");
        exit(0);
    }
    clientProcess(sockfd);
    exit(0);
}

void clientProcess(int fd){
    printf("connecting server successfully, you can input message now\n");
    char sendBuffer[1024],recvBuffer[1024];
    int n;
    memset(sendBuffer,0,1024);
    if(fgets(sendBuffer,sizeof(sendBuffer),stdin)!=NULL){
        send(fd,sendBuffer,sizeof(sendBuffer),0);
        /*memset(recvBuffer,0,1024);
        if(recv(fd,recvBuffer,1024,0) == 0)
            printf("No data have recevied \n");
        fputs(recvBuffer,stdout);*/
        memset(sendBuffer,0,1024);
    }
}

#endif
