#ifndef UDPSERVICE
#define UDPSERVICE

#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <memory.h>
#include <signal.h>

static void recvfromInt(int);
static int count = 0;


void  processServer(int fd,struct sockaddr *source,socklen_t len){
    int n,len_n = sizeof(n);
    getsockopt(fd,SOL_SOCKET,SO_RCVBUF,&n,(socklen_t*)&len_n);
    printf("rcvBuf len = %d \n", n);
    char recvBuff[1024];
    signal(SIGINT,recvfromInt);
    while(1){
        recvfrom(fd,recvBuff,1024,0,source,&len);
        ++count;
        /*
        memset(recvBuff,0,1024);
        if((n = recvfrom(fd,recvBuff,1024,0,source,&len))>=0){
            printf("Receive meg from client : %s", recvBuff);
            sendto(fd,recvBuff,n,0,source,len);
        }
        else{
            printf("No data \n");
        }*/
    }
}

static void recvfromInt(int signo){
    printf("received %d datagrams \n",count);
}

#endif
