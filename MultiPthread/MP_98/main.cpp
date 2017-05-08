#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "ThreadPool.h"

class MyTask: public Task{
public:
    MyTask(){
    }

    int run(){
        printf("thread[%lu] : %s \n" , pthread_self() , (char*)this->arg_);
        sleep(1);
        return 0;
    }
};

int main(){
    char str[] = "hello world";
    MyTask task;
    task.setArg((void*)str);
    threadPool threadPool(10);
    for(int i = 0 ;i < 100 ;++i){
        threadPool.addTask(&task);
    }
    while(1){
        printf("there are still %d tasks need to process \n", threadPool.size());
        if(threadPool.size() == 0){
            threadPool.stop();
            printf("Now exit");
            exit(0);
        }
        sleep(2);
    }
    return 0;
}
