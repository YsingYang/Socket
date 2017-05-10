#include "threadPool.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>


//测试的run函数,
    int run(int i, const char *p,int* arg){
        printf("thread[%lu] : (%d, %s)\n", pthread_self(), i , (char*)p);
        sleep(1);
        return i;
    }


int main(){
    threadPool tp(1000);
    int* arg = (int*)malloc(sizeof(int));

    for(int i = 0 ; i < 1000 ; ++i){
        tp.addTask(std::bind(run, i, "hello world" , arg));
    }

    while(1){
       if(tp.size() == 0){
            tp.stop();
            printf("Now I will exit\n");
            printf("argVal : %d \n", *arg);
            free(arg);
            return 0;
        }
    }

    return 0;
}
