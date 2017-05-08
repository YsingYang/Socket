#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include "UDPClient.h"

int main(){
    int sockfd ;
    struct sockaddr_in clientServer;
    sockfd = socket(AF_INET,SOCK_DGRAM,0);

    bzero(&clientServer,sizeof(clientServer));
    clientServer.sin_family = AF_INET;
    inet_pton(AF_INET,"127.0.0.1",&clientServer.sin_addr);
    clientServer.sin_port = htons(6666);
    clientProcess(sockfd,(sockaddr *)&clientServer,sizeof(clientServer));
}
