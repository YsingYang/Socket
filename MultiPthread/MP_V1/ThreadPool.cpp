#include "ThreadPool.h"

threadPool::threadPool(int num){
    //在线程池中创建相应数量的线程
    for(int i = 0 ; i < num ; ++i){
        TPool.push_back(new PNThread());
    }
    freeThread = num; //可用线程
    //创建一个主线程以维护线程池
    std::thread mainThread(&threadPool::run,this);
    mainThread.detach();
}

bool threadPool::addTask(baseTask *_task){ ///这样写锁会不会有问题
    locker.lock();
    if(taskQueue.size() >= MAXTASKWAITING){
        locker.unlock();
        return false;
    }
    else{
        taskQueue.push(_task);
        locker.unlock();
        return true;
    }
}

void threadPool::run(){
    while(true){
        locker.lock();
        //查找有无任务加入
        if(taskQueue.empty())
            continue;
        //有任务,轮询分配一个可空闲线程
        for(int i = 0 ; i < TPool.size() ; ++i){
            if(TPool[i] -> getStatus()){
                std::cout<<" 分配线程 :"<<i<<"处理该任务"<<std::endl;
                TPool[i] -> addTask(taskQueue.front()); ///假设添加失败如何处理
                taskQueue.pop();
                break;
            }
        }
        locker.unlock();
    }
}

threadPool::~threadPool(){
    for(int i = 0 ; i < TPool.size() ; ++i){
        delete TPool[i];
    }
}
