#include "ThreadPool.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

threadPool::threadPool(int threadNum){
    isRunning_ = true;
    threadsNum_ = threadNum;
    createThreads();
}

threadPool::~threadPool(){
    stop();
    for(auto i : taskQueue_){
        delete i;
    }
    taskQueue_.clear();
}

int threadPool::createThreads(){
    pthread_mutex_init(&mutex_, NULL);//初始化互斥锁
    pthread_cond_init(&condition_, NULL);//初始化条件变量
    thread_ = (pthread_t*)malloc(sizeof(pthread_t)*  threadsNum_);
    for(int i = 0 ; i< threadsNum_ ; ++i){
        pthread_create(&thread_[i], NULL, threadFunc, this);///最后那个this是干嘛的
        //新建线程, 传入threadFunc作为线程执行函数,
    }
    return 0;
}

size_t threadPool::addTask(Task *task){
    pthread_mutex_lock(&mutex_);
    taskQueue_.push_back(task);
    int size = taskQueue_.size();
    pthread_mutex_unlock(&mutex_);
    pthread_cond_signal(&condition_);///唤醒条件变量,告诉其有任务需要执行
}

void threadPool::stop(){
    if(!isRunning_){
        return ;
    }
    isRunning_ = false;
    pthread_cond_broadcast(&condition_);///唤醒线程
    for(int i = 0 ; i < threadsNum_ ; ++i){
        pthread_join(thread_[i], NULL);///等待线程执行完
    }

    free(thread_);
    thread_ = nullptr;

    pthread_mutex_destroy(&mutex_);
    pthread_cond_destroy(&condition_);
}

size_t threadPool::size(){///返回当前线程池的任务个数
    pthread_mutex_lock(&mutex_);
    int size = taskQueue_.size();
    pthread_mutex_unlock(&mutex_);
    return size;
}

Task* threadPool::take(){
    Task* task = nullptr;
    while(!task){
        pthread_mutex_lock(&mutex_);
        while(taskQueue_.empty() && isRunning_){
            pthread_cond_wait(&condition_,&mutex_);///当前没有任务,解开锁等待任务
        }
        if(!isRunning_){
            pthread_mutex_unlock(&mutex_);
            break;
        }
        else if(taskQueue_.empty()){ ///这里应该是进程池stop()强行释放锁,检测到没有任务 退出
            pthread_mutex_unlock(&mutex_);
            continue;
        }

        assert(!taskQueue_.empty());///保证不会出现空
        task = taskQueue_.front();
        taskQueue_.pop_front();
        pthread_mutex_unlock(&mutex_);
    }
    return task;
}

void* threadPool::threadFunc(void *arg){
    pthread_t tid = pthread_self();///返回线程自身的tid
    threadPool* pool = static_cast<threadPool*>(arg);
    while(pool->isRunning_){
        Task* task = pool->take();///读取队列任务
        if(!task){
            printf("thread %lu will exit\n" , tid);
            break;
        }
        assert(task);
        task->run();
    }
    return 0;
}
