#ifndef UDPCLIENT
#define UDPCLIENT

#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>


void clientProcess(int fd,struct sockaddr *dest , socklen_t len){
    char sendBuff[1024];
    for(int i = 0 ; i < 100000 ; ++i){ ///洪水发送到服务器中,出现UDP丢失,
        sendto(fd,sendBuff,1024,0,dest,len);
    }
    /*int n;
    char sendBuff[1024],recvBuff[1024];
    printf("prepare for sending message\n");
    ///调用connect,利用send ,recv进行UDP发送与接受 ,也可以利用sendto , recvfrom不过不要指定后面两个参数
    signal(SIGINT,recvfromInt);
    connect(fd,dest,len);
    while(fgets(sendBuff,1024,stdin) >= 0){
        //n = sendto(fd,sendBuff,strlen(sendBuff),0,dest,len);
        n = send(fd,sendBuff,strlen(sendBuff),0);
        //if(recvfrom(fd,recvBuff,1024,0,NULL,NULL) >= 0)
        if(recv(fd,recvBuff,1024,0) >=0)
            printf("recv from Server %s",recvBuff);
        else
            exit(0);
        memset(sendBuff,0,1024);
        memset(recvBuff,0,1024);
    }*/
}


#endif
