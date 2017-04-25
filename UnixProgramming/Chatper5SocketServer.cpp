#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include "server.h"
#include "Client.h"
#include<iostream>

using namespace std;


int main(){
        pid_t pid = fork();
        if(pid == 0){
            client();
        } //child process
        else{
            server();
            wait(0);
        }
        return 0;
}
