#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<iostream>

#define MAXLINE 4096

using namespace std;

int main(int argc, char** argv){
    int sockfd,connfd;
    ///定义sockfd与用于建立服务器绑定文件描述符，connfd用于处理连接请求的描述符
    struct sockaddr_in server_addr;
    char buff[4096];///用于接收的缓冲区
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0){
        printf("socket error\n");
        return -1;
    }
   /// bzero(&server_addr,sizeof(server_addr));
   memset(&server_addr,0,sizeof(server_addr));///似乎同上句类似.该函数需要传入一个引用
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = htonl(INADDR_ANY);///host to next (long)
   server_addr.sin_port = htons(6667);///设立监听端口

    ///绑定文件描述符
    if(bind(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr))==-1){
        printf("bind socket error :");
        return -1;
    }
    if(listen(sockfd,10)==-1){
        printf("listen socket error ");
        return -1;
    }

    printf("=========waiting for client's request======\n");
    while(1){
        cout<<"watring "<<endl;
        if((connfd = accept(sockfd, (struct sockaddr*)NULL, NULL)) == -1 ){
            printf("accept socket error");///这里设成NULL没有问题吗？
            continue;
        }
      //  connfd = accept(sockfd, (struct sockaddr*)NULL, NULL);
        cout<<"connect"<<endl;
        while(int n = recv(connfd,buff,MAXLINE,0)){
           // int n = recv(connfd,buff,MAXLINE,0);///recv为接收返回的套接字长度,如果为-1则发生错误
           buff[n] = '\0';///记得用的是char[],需要将\n补上
            printf("recv msg from client: %s\n", buff);
           // cout<<buff<<endl;
            sprintf(buff,"%d bytes altogether\n",n);
            cout<<(int)strlen(buff)<<endl;
            write(connfd,buff,strlen(buff));
        }
        close(connfd);///记得close刚刚accept用的fd;
    }
    close(sockfd);
}
