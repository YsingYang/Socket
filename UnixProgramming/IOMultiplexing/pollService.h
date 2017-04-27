#ifndef POLLSERVICE
#define POLLSERVICE
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


#define PORT 6666///指定端口号

//void processServer();
void runService();


void runService(){
    int maxIndex , listenFD , connFD;
    int nready ; ///用于epoll返回值,确定有多少个FD就绪

    size_t n;
    char buff[MAXLINE];

    socklen_t clientLen;
    struct sockaddr_in clientAddr,serverAddr;
    struct pollfd client[4];
    listenFD = socket(AF_INET,SOCK_STREAM,0);
    bzero(&serverAddr,sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);
    bind(listenFD,(struct sockaddr*)&serverAddr,sizeof(serverAddr));

    client[0].fd = listenFD;///将监听描述符传给client[0] , 用于IO复用中查看是否就绪
    client[0].events = POLLRDNORM; /// 普通数据可读 ReaDNORMal ,应该也可以设置为POLLIN

    for(int i = 1; i < 4 ; ++i){
        client[i].fd = -1;
    }
    maxIndex  = 0 ;///表示当前最大可用套接字描述符为0

    listen(listenFD,10); ///开启listen
    /***
        下面开始对FD进行处理
    ***/
    printf("***************** waiting  ****************\n ");
    while(1){
        nready = poll(client,maxIndex +1, -1); ///poll timeout设置为负数则为INF;无限等待
        if(client[0].revents & POLLRDNORM){ ///如果时间为POLLRDNORM ,这里不是&&吗,
            clientLen = sizeof(clientAddr);
            connFD = accept(listenFD,(struct sockaddr *)&clientAddr,&clientLen);
            printf("A client connecting  \n ");
            int i = 1;
            for(;i < 4; ++i){
                if(client[i].fd == -1){
                    client[i].fd = connFD;
                    break;
                    ///不用设置events吗？
                }
            }
            if(i == 4){ /// 连接已达到最大请求，不能加入新的连接
                printf("too many clients\n");
                close(connFD);
            }
            client[i].events = POLLRDNORM;
            maxIndex = std::max(maxIndex,i);

            if(--nready <= 0) ///当前poll就绪的数量已处理完
                continue;
        }

        for(int i = 1 ; i < 4 ; ++i){  ///对每个与客户端连接的套接字描述符进行处理
            if(client[i].fd < 0)
                continue;
            int fd = client[i].fd;
            if(client[i].revents & (POLLRDNORM| POLLERR)){ ///假设是当中的一个事件
                if( (n = read(fd,buff,MAXLINE)) < 0){
                    if(errno == ECONNRESET){ ///如果收到的是RESET,断开连接
                        close(fd);
                        printf("%d, client disconnected \n", fd);
                        client[i].fd = -1;
                    }
                    else{
                        printf("read error\n ");
                    }
               }
               else if( n == 0 ){ ///如果接收的数据量为0 证明发送的是FIN
                    printf("%d, client disconnected \n", fd);
                    close(fd);
                    client[i].fd = -1;
               }
               else{
                    //write(fd,buff,n);///将数据写入buff中;
                    printf("Received msg : %s", buff);
               }
               if(--nready <= 0){
                    break;
               }

            }
        }
    }
}

/***
    是否需要添加信号?
***/





#endif
