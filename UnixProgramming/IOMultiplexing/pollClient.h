#ifndef POLLCLIENT
#define POLLCLIENT

#define MAXLINE 2048

#include<algorithm>
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
#include <limits.h> ///OPEN_MAX
#include<errno.h>
#include<sys/poll.h> ///poll头文件

void runClient(){
    int connectFd , maxIndex ;
    struct sockaddr_in clientAddr;
    struct pollfd pollSet[10]; ///给客户端建立10个poll数组用于监听相应的套接字
    connectFd = socket(AF_INET,SOCK_STREAM,0);

    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(6666); ///放入想要连接的服务端端口,这里跟服务器不一样不是自己的端口
    inet_pton(connectFd,"127.0.0.1",&clientAddr.sin_addr);

    pollSet[1].fd = connectFd;///端口监听
    pollSet[1].events = POLLRDNORM;

    pollSet[0].fd = STDIN_FILENO;///标准输入
    pollSet[0].events = POLLRDNORM;
    maxIndex = 1;
    for(int i = 2 ; i < 10 ;++i){
        pollSet[i].fd = -1;
    }
    while(connect(connectFd,(struct sockaddr*)&clientAddr,sizeof(clientAddr))<0){
        printf("connect error ");
        sleep(30);///连接失败后30秒后重新连接
    }
    char recvBuff[1024] , sendBuff[1024];
    int flag = 1 ,n = 0;
    while(flag){
        printf("建立连接\n");
        int nread = poll(pollSet,maxIndex + 1 , -1);
        if(nread <= 0)
                continue;
        if(pollSet[0].revents & POLLRDNORM){
            if((n = read(pollSet[0].fd,sendBuff,1024))<0){
                printf("sending error \n");
            }
            else if(n == 0){
                printf("%s",sendBuff);
                printf("close connection\n");
                ///写入FIN?
                flag = 0;
            }
            else{
                write(connectFd,sendBuff,strlen(sendBuff));
            }
        }
        for(int i = 1 ; i < 10 ; ++i){
            if(pollSet[i].fd < 0)
                    continue;
            int fd = pollSet[i].fd;
            if(pollSet[i].revents & (POLLRDNORM | POLLERR)){
                if((n = read(fd,recvBuff,1024)) < 0){
                        if(errno == ECONNRESET){///客户端断开连接时
                            pollSet[i].fd = -1;
                            close(fd);
                            printf("disConnected  on recv ECONNRESET \n" );
                        }
                        else{
                            printf("read error \n");
                        }
                        flag = 0;
                }
                else if(n == 0 ){
                    pollSet[i].fd = -1;
                    close(fd);
                    printf("disConnected  on recv FIN \n" );
                    flag = 0;
                }
                else{
                    printf("recvMsg %s " , recvBuff);
                }
                if(--nread <=0)
                        break;
            }
        }

    }

}

#endif // POLLCLIENT
