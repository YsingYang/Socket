#ifndef SERVER.H

#define  SERVER.H
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

void sig_child(int );///信号处理函数声明
void ServerProcess(int );


void server(){
    int listenfd , connfd;
    pid_t c_pid;
    sockaddr_in server_addr,client_addr;
    socklen_t client_len;
    listenfd = socket(AF_INET,SOCK_STREAM,0);
    bzero((void*)&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(6666);///连接端口号6666
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(listenfd,(sockaddr * )&server_addr,sizeof(server_addr));
    listen(listenfd,5);//后面参数为listen队列最大长度
    signal(SIGCHLD,sig_child);///处理信号的类型 + 处理信号的函数指针
    printf("--------server start up successful -----------\n");
    while(1){
        client_len = sizeof(client_addr);
        if((connfd = accept(listenfd,(sockaddr *)&client_addr,&client_len))<0){///3次握手出错
            printf("error \n");
            if(errno == EINTR){///系统调用中断
                continue;
            }
            else{
                printf("accept error\n");
            }
        }
        printf("Client  connected \n ");
        if((c_pid = fork())==0){///child process
            close(listenfd);
            ServerProcess(connfd);
            exit(0);
        }
        close(connfd);
    }
}

void ServerProcess(int fd){
    printf("%d Client communicating \n",fd);
    char sendBuffer[1024],recvBuffer[1024];
    int n;
    while((n = recv(fd,recvBuffer,sizeof(recvBuffer),0))>0){
        printf("%s", recvBuffer);
        memset((void*)sendBuffer,0,sizeof(sendBuffer));
        memset(recvBuffer,0,1024);
        //sprintf(sendBuffer,"connect successful");
        //send(fd,sendBuffer,strlen(sendBuffer),0);
    }
}


void sig_child(int signo){///信号编号是怎么传到这个函数中的
    pid_t pid;
    int stat; /// 获取状态
    while((pid = waitpid(-1,&stat,WNOHANG))>0){///waitpid 返回pid  Pid=-1,等待任一个子进程。与wait等效
        /**
        Pid=-1,等待任一个子进程。与wait等效。

        Pid>0.等待其进程ID与pid相等的子进程。

        Pid==0等待其组ID等于调用进程组ID的任一个子进程。

        Pid<-1等待其组ID等于pid绝对值的任一子进程。
        */
        printf("Child %d process terminated \n ",pid);
    }
}
#endif
