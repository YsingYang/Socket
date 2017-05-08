#include "threadPool.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
//class myTask{
//public:
    //myTask(){}

    int run(int i, const char *p,int* arg){
        printf("thread[%lu] : (%d, %s)\n", pthread_self(), i , (char*)p);
        *arg = i;
        sleep(1);
        //std::cout<<arg<<std::endl;
        return i;
    }
//};

int main(){
    /*threadPool tp(3);
    int* arg = (int*)malloc(sizeof(int));

    for(int i = 0 ; i < 5 ; ++i){
        tp.addTask(std::bind(run, i, "hello world" , arg));
    }
    sleep(5);
    printf("continue \n");
    tp.stop();
    for(int i = 0 ; i < 5 ; ++i){
        tp.addTask(std::bind(run, i, "hello world" , arg));
    }
    while(1){
        //printf("there are still %d tasks need to process\n", tp.size());
        if(tp.size() == 0){
            tp.stop();
            printf("Now I will exit\n");
            printf("argVal : %d \n", *arg);
            free(arg);
            return 0;
        }
        sleep(0);
    }*/

    int pid , val = 0;
    if((pid = fork()) == 0){
        val = 1;
        std::cout<<"sub: "<<val<<std::endl;
        while(1){
            ;
        }
    }
    else{
        sleep(2);

        printf("%d", val);
    }
    return 0;
}
