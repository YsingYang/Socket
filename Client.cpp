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
#define MAXLINE 4096
#define PORT 6667
using namespace std;

void process_conn_client(int& s);

int main(int argc, char** argv){
    int s;
    struct sockaddr_in server_addr;

    s = socket(AF_INET,SOCK_STREAM,0);
    if(s<0){
        printf("socket error\n");
        return -1;
    }

    bzero(&server_addr,sizeof(server_addr));//memset
    server_addr.sin_family = AF_INET;///跟建立socket时一样
 ///   server_addr.sin_addr.s_addr = htonl(INADDR_ANY);///本地地址
    server_addr.sin_port   = htons(PORT);

    inet_pton(AF_INET,argv[1],&server_addr.sin_addr);

    connect(s,(struct sockaddr*)&server_addr,sizeof(server_addr));
     ssize_t size = 0;
    char buffer[1024];

    process_conn_client(s);///客户端对套接字的处理

    close(s);
    return 0;
}

void process_conn_client(int &s){///这个s只是一个整型，只不过再内核中绑定了响应的socket服务
    ssize_t size = 0;
    char buffer[1024];

    while(1){
        size = read(0,buffer,1024);///read()会把参数fd 所指的文件传送count 个字节到buf 指针所指的内存中.0表示标准输入读取
        buffer[size] = '\0';///对于c程序来说，buffer接收到字符串后最好记得+‘、0’;
        cout<<buffer<<endl;

        if(size>0){
            write(s,buffer,size);
            size = read(s,buffer,1024);

            write(1,buffer,size);///将读取到的写到标准输出中
        }

    }
}
