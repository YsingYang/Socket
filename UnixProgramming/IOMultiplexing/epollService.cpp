#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> //sockaddr_in
#include <arpa/inet.h> //inet
#include <sys/epoll.h>
#include <fcntl.h>

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

const int MAXEPOLLSIZE = 10000;
const int MAXLINE = 10240;
const int PORT = 6666;

int handle(int connfd);

inline int setNonblock(int sockfd){
    if(fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1){
        return -1;
    }
    return 0;
}

int main(int argc, char **argv){
    //int listenq = 1024; //listen监听队列

    int listenfd, connfd, epfd, nfds, curfds, acceptCount = 0;
    /**
        listen fd : 监听fd, conndfd : 连接fd
        epfd : epoll句柄 ,
        curfds  : 当前受 epoll监听事件的数量
        nfds : 当前epoll接收到的最大套接字的FD
        acceptCount : 接收连接的总数
    **/
    struct sockaddr_in servAddr, clientAddr;
    socklen_t sockLen = sizeof(servAddr);

    struct epoll_event ev;
    struct epoll_event events[MAXEPOLLSIZE];
    //struct rlimit rt;
    char buf[MAXLINE];

    bzero(&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(PORT);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd == -1){
        perror("cann't create socket file\n");
        return -1;
    }

    if(setNonblock(listenfd) < 0 ){
        perror("setnonblock error\n");
    }

    if(bind(listenfd, (struct sockaddr *)&servAddr, sizeof(servAddr))< 0){
        perror("bind error\n");
        return -1;

    }

    if(listen(listenfd, 10) == -1){
        perror("listen error\n");
        return -1;
    }

    epfd = epoll_create(MAXEPOLLSIZE);///创建epoll句柄
    ev.events = EPOLLIN | EPOLLET;
    ///设置边缘触发, 与监听接受事件
    ev.data.fd = listenfd;

    if(epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) < 0){
        fprintf(stderr, "epoll set error fd %d\n", listenfd);
    }///设置listenfd ,进epoll
    curfds = 1;
    printf("epollServer start port : %d, max connection is %d, \n",PORT, MAXEPOLLSIZE);

    while(1){
        nfds = epoll_wait(epfd, events, curfds, -1);
        /// int epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout);
        if(nfds == -1){
            perror("epoll_wait");
            continue;
        }
        for(int i = 0 ; i < nfds ; ++i){
                if(events[i].data.fd == listenfd){ ///若该时间的fd为listenfd , 新建一个connfd
                    connfd = accept(listenfd, (struct sockaddr*)&clientAddr, &sockLen);
                    if(connfd < 0){
                        perror("accept error \n");
                        continue;
                    }
                    sprintf(buf, "accept from %s :%d \n", inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port);
                    printf("%d : %s", acceptCount, buf);

                if(curfds >= MAXEPOLLSIZE){
                    fprintf(stderr, "too many connection more than %d\n", MAXEPOLLSIZE);
                    close(connfd) ;///不再接受监听
                    continue;
                }

                if (setNonblock(connfd) < 0) {
                    perror("setnonblocking error");
                }

                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = connfd;
                if(epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev) < 0){
                    fprintf(stderr, "add socket '%d' to epoll failed: %s\n", connfd, strerror(errno));
                        return -1;
                }
                curfds ++;
                continue;
            }
            if(handle(events[i].data.fd) <= 0){
                epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, &ev); ///如果接收到的客户端请求< 0 证明断开连接,删除相应监听fd时间
                --curfds ;
        }
        }
    }
    close(listenfd);
    close(epfd);
    return 0;

}

int handle(int connfd){
    int nread;
    char buf[MAXLINE];
    nread = read(connfd, buf, MAXLINE);
    if(nread == 0){
        printf("client close the connection\n");
        close(connfd);
        return -1;
    }
    if(nread < 0){
        perror("read error");
        close(connfd);
        return -1;
    }
    printf("%s ", buf);
    if(write(connfd, buf, nread)<0)
        printf("sending error\n");
    return 1;
}
