#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<iostream>
#include<sys/uio.h>
#include<fcntl.h>

#define MAXLINE 4096
#define PORT 6666
using namespace std;

void process_conn_client(const int& s);

int main(){
    int s;
    struct sockaddr_in server_addr;
    s = socket(AF_INET,SOCK_STREAM,0);
    if(s<0){
        printf("socket error\n");
        return -1;
    }

    bzero(&server_addr,sizeof(server_addr));//memset
    server_addr.sin_family = AF_INET;///跟建立socket时一样
    //const char * strptr = "127.0.0.1";
    //struct in_addr  *addrptr;
    //inet_aton(strptr,addrptr);
    //server_addr.sin_addr.s_addr = addrptr->s_addr;///本地地址
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port   = htons(PORT);

    //inet_pton(AF_INET,argv[1],&server_addr.sin_addr);

    int flags = fcntl(0, F_GETFL, 0);
    fcntl(0, F_SETFL, flags | O_NONBLOCK);


    if(connect(s,(struct sockaddr*)&server_addr,sizeof(server_addr))<0)
        cout<<"error "<<endl;
     ssize_t size = 0;
    process_conn_client(s);///客户端对套接字的处理

    close(s);
    return 0;
}

void process_conn_client(const int &s){///这个s只是一个整型，只不过再内核中绑定了响应的socket服务
    ssize_t size = 0;
    char buffer[1024];
    char recvBuffer[1024];
    /**
        等待客户端发送回复,如果stack没有足够的空间,会有0,否则回应successful
        似乎不能使用非阻塞,不然的话客户端跑得比服务器快的话,recv就已经过了还没有收到相应的回复
    **/
    memset((void*)recvBuffer,0,1024);///记得在使用前先memset一下,否则很容易有乱码
    if(recv(s,recvBuffer,1024,0) == 0){
                printf("the other side has been closed \n");
                return ;
    }
    printf("%s",recvBuffer);
    while(1){
        /**
            接收从服务器发送过来的数据
        */
        memset((void*)recvBuffer,0,sizeof(recvBuffer));
        if(recv(s,recvBuffer,1024,MSG_DONTWAIT)>0)
            printf("%s",recvBuffer);


        /**
            准备发送给服务器的数据
        **/
        memset((void*)buffer,0,sizeof(buffer));
        size = read(0,buffer,1024);///read()会把参数fd 所指的文件传送count 个字节到buf 指针所指的内存中.0表示标准输入读取
        ///buffer[size] = '\0';///对于c程序来说，buffer接收到字符串后最好记得+‘、0’;
        if(size>0){
            send(s,buffer,strlen(buffer),0);
        }
    }
}

