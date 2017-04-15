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
#include<vector>
#include<unordered_map>
#include<linux/tcp.h>///tcp_info
#define MAXLINE 4096
/***
缺陷 1 . 如何在stack中没有空余位置时发送给客户端,让客户端直接终止
          2 . 我希望能存储客户端更多的信息,IP地址+ 端口
          3 . 超过相应的字符长度应该怎么处理?
          4 . 客户端show返回是被read(0)给阻塞了

***/
using namespace std;

static struct iovec *vs = NULL;///定义一个全局的变量vs,用来接管malloc出的iovc

bool getConnectionState(int fd);
void sig_process(int sign);
void sig_pipe(int sign);
void ReplyState2Client(const int &fd);
void requestUserName(const int &fd);
void showOnlineUser(const int &fd);
void ReplyCMCfail(const int &fd);
void sendConnectSuc(const int &sfd,const int &dfd);
//void process_conn_server(const int &);

unordered_map<int,vector<string>> user;///全局变量,用于记录fd->user 端口+地址的映射
unordered_map<string,int> getUserFd;
unordered_map<string,string> communication;

int main(){
    int sockfd,connfd;
    ///定义sockfd与用于建立服务器绑定文件描述符，connfd用于处理连接请求的描述符
    struct sockaddr_in server_addr,client_addr;
    socklen_t client_len;
    client_len = sizeof(client_addr);
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
        usleep(10000);
        /**
            轮巡查找是否有已经断开了的客户端(意外断开与断开一起)
        **/
        for(int i=0;i<5;++i){
            if(connection[i]!=0&&!getConnectionState(connection[i])){
                freefd.push(i);
                close(connection[i]);
                connection[i] = 0;
                cout<<"Connection   ["<<i<<  "]  disconnect  push it into stack "<<endl;///为了效率的话可以放再fd>0里面的
            }
        }
        /**
            非阻塞accept,如果connfd>0,表示有连接进入,对当前连接进行处理,同时将它保存在一个数组中.
        **/
        connfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);///这里的client_len要传入引用
        if(connfd>0){
            if(freefd.empty()){
                printf("Not enough space to add connection \n");
                close(connfd);
                continue;
            }
            cout<<"Client  "<< freefd.top() <<" connect succsss"<<endl;
            ReplyState2Client(connfd);
            connection[freefd.top()] = connfd;
            freefd.pop();
            /***
                下为输出测试,测试是否能将客户端连接的ip以及端口正常的输出出来.
            **/
            //char *userAddr;
            struct in_addr ClientAddr;
            ClientAddr.s_addr = client_addr.sin_addr.s_addr;
            string userAddr(inet_ntoa(ClientAddr));

            user[connfd] = {"",userAddr,to_string(ntohs(client_addr.sin_port))};
            /***
                请求客户端发送username 以便记录
            **/
            requestUserName(connfd);
       }

       /***
          轮巡是否有客户端给服务器发送了信息.
       */
        for(int i=0;i<5;++i){
            int n;
            memset((void*)buff,0,sizeof(buff));
            if((n = recv(connection[i],buff,MAXLINE,MSG_DONTWAIT))>0){///虽说收到会自动加上一个[\n]
                if(user[connection[i]][0].empty()){ ///用户未设置用户名
                    string userName = string(buff,n-1);
                    user[connection[i]][0] = userName;
                    getUserFd[userName] = connection[i];///建立user->fd的映射关系
                    continue;
                }
                else if(strcmp(buff,"show\n")==0){
                    showOnlineUser(connection[i]);
                }
                else{///待补充其他接受返回函数
                    string recvMsg(buff,n-1);
                    if(recvMsg.substr(0,3) == "cmc"){///开启communicate
                        string CMCobj =  recvMsg.substr(4);
                       // cout<<CMCobj<<"  "<<CMCobj.size()<<" back element "<<CMCobj.back()<<endl;
                        if(getUserFd.find(CMCobj) == getUserFd.end()){///如过client想要交流的obj不存在,返回一个fail信息
                            ReplyCMCfail(connection[i]);
                            continue;
                        }
                        /***
                            处理客户端之间的连接
                        */
                        communication[CMCobj] = user[connection[i]][0];
                        communication[user[connection[i]][0]] = CMCobj;///服务器已为双方完成搭建

                        sendConnectSuc(connection[i],getUserFd[CMCobj]);

                        cout<<"connection successful "<<endl;
                    }
                    else{
                        printf("recv msg from client[%d]: %s",i, buff);///正常的接受数据
                    }
                }
            }
        }
    }
    close(sockfd);
}

/***
    查看连接状态
**/

void showOnlineUser(const int &fd){
    string tmp;
    for(auto iter:user){
        tmp += iter.second[0] + '\n';
    }
    int size = tmp.size();
    send(fd,(void *)const_cast<char *>(tmp.c_str()),size,0);
}

void sendConnectSuc(const int &sfd,const int &dfd){
    char buff[50];
    sprintf(buff,"set your Communication with  %d successfully ",dfd);
    send(sfd,buff,strlen(buff),MSG_DONTWAIT);
    memset((void*)buff,0,50);
    sprintf(buff,"set your Communication with  %d successfully ",sfd);
    send(dfd,buff,strlen(buff),MSG_DONTWAIT);
}

bool getConnectionState(int fd){
    struct tcp_info info;
    int infoLen = sizeof(info);
    getsockopt(fd,IPPROTO_TCP,TCP_INFO,&info,(socklen_t*)&infoLen);
    if((info.tcpi_state==1))
        return true;
    return false;
}
/***
    回复客户端连接成功的状态
***/
void ReplyState2Client(const int &fd){
    char buff[30];
    sprintf(buff,"Connect successful\n");
    printf("%s",buff);
    send(fd,buff,strlen(buff),MSG_DONTROUTE);
}

void ReplyCMCfail(const int &fd){
    char buff[30];
    memset((void*)buff,0,30);
    sprintf(buff,"User don't exist ");
    send(fd,buff,strlen(buff),0);
}

void requestUserName(const int &fd){
    char buffer[32];
    sprintf(buffer,"Please input your username");
    printf("%s\n",buffer);
    send(fd,buffer,strlen(buffer),MSG_DONTWAIT);///这里设置为非阻塞有没有问题
   /* char recvBuffer[32];
    int size;
    if((size = recv(fd,recvBuffer,32,0))>0&&user[fd][0].empty()){
        user[fd][0] = string(recvBuffer,size-1);///记得减1,发送过来的有一个是'\0'
    }*/
}

void setUserName(const int &fd){
    char recvBuffer[32];
    int size;
    if((size=recv(fd,recvBuffer,32,MSG_DONTWAIT))>0){
        user[fd][0] = string(recvBuffer,size-1);
    }
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


