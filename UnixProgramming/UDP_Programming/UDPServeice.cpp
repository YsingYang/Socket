#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include"UDPService.h"

int main(){
    int sockfd;
    struct sockaddr_in serverAddr  , clientAddr ;
    sockfd = socket(AF_INET,SOCK_DGRAM,0);

    bzero(&serverAddr,sizeof(serverAddr));
    bzero(&clientAddr,sizeof(clientAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(6666);

    bind(sockfd,(sockaddr*)&serverAddr,sizeof(serverAddr));
    printf( " --------------------waiting for client  ----------------------------------\n");
    processServer(sockfd,(sockaddr*)&clientAddr,sizeof(clientAddr));
    return 0;
}

