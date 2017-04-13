#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<iostream>
#include<signal.h>
#include<wait.h>
#include<sys/uio.h>///这个h是干嘛的
#include<stack>
#include<fcntl.h>
#include<linux/tcp.h>///tcp_info
#define MAXLINE 4096

using namespace std;

static struct iovec *vs = NULL;///定义一个全局的变量vs,用来接管malloc出的iovc

bool getConnectionState(int fd);
void sig_process(int sign);
void sig_pipe(int sign);
//void process_conn_server(const int &);


int main(){
    int sockfd,connfd;
    ///定义sockfd与用于建立服务器绑定文件描述符，connfd用于处理连接请求的描述符
    struct sockaddr_in server_addr;
    ///char buff[4096];///用于接收的缓冲区

    ///挂载信号
   // signal(SIGINT,sig_process);
    //signal(SIGPIPE,sig_pipe);

    sockfd = socket(AF_INET,SOCK_STREAM,0);
    char buff[100];
    if(sockfd<0){
        printf("socket error\n");
        return -1;
    }

    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    ///设置为非阻塞


   /// bzero(&server_addr,sizeof(server_addr));
   memset(&server_addr,0,sizeof(server_addr));///似乎同上句类似.该函数需要传入一个引用
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr =INADDR_ANY;///host to next (long)
   server_addr.sin_port = htons(6666);///设立监听端口

    ///绑定文件描述符
    if(bind(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr))==-1){
        printf("bind socket error :");
        return -1;
    }
    if(listen(sockfd,10)==-1){
        printf("listen socket error ");
        return -1;
    }
    int connection[5] = {0,0,0,0,0} ;
    stack<int> freefd;
    for(int i=0;i<5;++i){
        freefd.push(i);
    }
    printf("=========waiting for client's request======\n");
    while(1){
        //cout<<"watring "<<endl;
        for(int i=0;i<5;++i){
            if(connection[i]!=0&&!getConnectionState(connection[i])){
                freefd.push(i);
                close(connection[i]);
                connection[i] = 0;
                cout<<"Connection   ["<<i<<  "]  disconnect  push it into stack "<<endl;///为了效率的话可以放再fd>0里面的

            }
        }
        connfd = accept(sockfd, (struct sockaddr*)NULL, NULL);
        if(connfd>0){
            if(freefd.empty()){
                close(connfd);
                continue;
            }
            cout<<"connect succsss"<<endl;
            if(connfd>0){
                connection[freefd.top()] = connfd;
                freefd.pop();
            }
            cout<<connfd<<"  "<<freefd.size()<<endl;
       }
        for(int i=0;i<5;++i){
            if(int n = recv(connection[i],buff,MAXLINE,MSG_DONTWAIT)>0){
            //int n = recv(connfd,buff,MAXLINE,0);///recv为接收返回的套接字长度,如果为-1则发生错误
          //  buff[n] = '\0';///记得用的是char[],需要将\n补上
                printf("recv msg from client: %s  \n", buff);
            /*
            sprintf(buff,"%d bytes altogether\n",n);
            cout<<(int)strlen(buff)<<endl;
            write(connfd,buff,strlen(buff));*/
            }
        }
        //close(connfd);///记得close刚刚accept用的fd;
    }
    close(sockfd);
}

bool getConnectionState(int fd){
    struct tcp_info info;
    int infoLen = sizeof(info);
    getsockopt(fd,IPPROTO_TCP,TCP_INFO,&info,(socklen_t*)&infoLen);
    if((info.tcpi_state==1))
        return true;
    return false;
}

void sig_process(int sign){
    printf("Catch a signal \n");
    free(vs);
    exit(0);
}

void sig_pipe(int sign){
    printf("Catch a signal\n");
    free(vs);///这里不会double free把。是两个信号处理方式不同吗？
    exit(0);
}

/*void process_conn_server(const int &fd){
    char buffer[30];///分配大小为30个char的数组作为存储空间
    ssize_t size = 0;

    struct iovec *v = (struct iovec*)malloc(3*sizeof(struct iovec));

    if(v==NULL){
        printf("Not enough memory\n");///如果没有足够的内存，malloc失败
    }

    vs = v;
    v[0].iov_base = buffer;
    v[1].iov_base = buffer +10;
    v[2].iov_base = buffer + 20;///这样子存储很容易造成溢出把.
    v[0].iov_len = v[1].iov_len = v[2].iov_len = 10;

    for(;;){
        size = readv(fd,vs,1);///最后的3表示缓冲区的大小
        if(size==0)
                return;

        printf("%s",(char*)v[0].iov_base);

        sprintf((char*)(v[0].iov_base),"%d  ",size);
        sprintf((char*)(v[1].iov_base),"bytes alt ");
        sprintf((char*)(v[2].iov_base),"ogether\n");

        v[0].iov_len = strlen((char*)v[0].iov_base);
        v[1].iov_len = strlen((char*)v[1].iov_base);
        v[2].iov_len = strlen((char*)v[2].iov_base);

        writev(fd,vs,3);///放松回客户端
    }
}*/
