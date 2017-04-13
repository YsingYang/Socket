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

    if(connect(s,(struct sockaddr*)&server_addr,sizeof(server_addr))<0)
        cout<<"error "<<endl;
     ssize_t size = 0;
    char buffer[1024];

    process_conn_client(s);///客户端对套接字的处理

    close(s);
    return 0;
}

void process_conn_client(const int &s){///这个s只是一个整型，只不过再内核中绑定了响应的socket服务
    ssize_t size = 0;
    char buffer[1024];

    while(1){
        size = read(0,buffer,1024);///read()会把参数fd 所指的文件传送count 个字节到buf 指针所指的内存中.0表示标准输入读取
        buffer[size] = '\0';///对于c程序来说，buffer接收到字符串后最好记得+‘、0’;
        if(size>0){
            send(s,buffer,size,0);
        }
    }
}


/*void process_conn_client(const int &s){
    char buffer[30];
    ssize_t size = 0;

    struct iovec *v = (struct iovec*)malloc(3*sizeof(iovec));///默认就是10吗？

    if(v==NULL){
        printf("Not enough memory\n");
        return ;
    }

    vc = v;

    v[0].iov_base = buffer;
    v[1].iov_base = buffer +10;
    v[2].iov_base = buffer +20;

    v[0].iov_len = v[1].iov_len = v[2].iov_len = 10;
    int i = 0;
    for(;;){
        size = read(0,(char *)v[0].iov_base,10);///读取10个字符
        char * end = v[0].iov_base+size;
        *end = '\0';
        if(size>0){
            v[0].iov_len = size;
            write(s,v,1);///只写了1个size的iovc
            v[0].iov_len = v[1].iov_len = v[2].iov_len = 10;///这样的结构很费啊

            size = readv(s,v,3);///从客户端中收到数据
            for(int i=0;i<3;++i){
                if(v[i].iov_len>0)
                    write(1,v[i].iov_base,v[i].iov_len);

            }
        }


    }

}*/
