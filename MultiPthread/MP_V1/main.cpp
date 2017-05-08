#include "ThreadPool.h"
#include "BaseTask.h"
#include "PNThread.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(){
    threadPool tp(3);
    baseTask t1(1);
    baseTask t2(2);
    baseTask t3(3);
    int x =1;;
    while(x < 10){
        tp.addTask(&t1);
        tp.addTask(&t2);
        tp.addTask(&t3);
        ++x;
    }
    sleep(1);
    return 0;
}
