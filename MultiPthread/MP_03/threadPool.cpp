#include "threadPool.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

//构造函数
threadPool::threadPool(int threadNum){
    isRunning_ = true;
    threadsNum_ = threadNum;
    //创建相应数量的线程
    createThreads();
}

threadPool::~threadPool(){
    stop();
}

//创建相应数量的线程
int threadPool::createThreads(){
    pthread_mutex_init(&mutex_, NULL);
    pthread_cond_init(&condition_, NULL);
    threads_ = (pthread_t*)malloc(sizeof(pthread_t) * threadsNum_);

    for(int i = 0 ; i < threadsNum_ ; ++i){
        pthread_create(&threads_[i] , NULL, threadFunc, this);
    }

    return 0;
}

//停止线程池, 等待每一个线程join,同时释放掉线程,和mutex,condition变量
void threadPool::stop(){
    if(!isRunning_){
        return;
    }
    isRunning_ = false;
    pthread_cond_broadcast(&condition_);//让所有条件变量释放

    for(int i = 0 ; i < threadsNum_ ; ++i){
        pthread_join(threads_[i],NULL);///第二个参数可以用来获取线程执行函数得到的返回值
    }
    free(threads_);
    threads_ = nullptr;

    pthread_mutex_destroy(&mutex_);
    pthread_cond_destroy(&condition_);
}

//添加任务仅任务队列
size_t threadPool::addTask(const Task& task){
    pthread_mutex_lock(&mutex_);

    taskQueue_.push_back(task);
    int size = taskQueue_.size();

    pthread_cond_signal(&condition_);
    pthread_mutex_unlock(&mutex_);
    return size;
}

int threadPool::size(){
    pthread_mutex_lock(&mutex_);
    int size = taskQueue_.size(); //获取任务队列大小
    pthread_mutex_unlock(&mutex_);
    return size;
}

//取任务队列中的任务,若队列中没有任务,阻塞在condition中, 同时释放mutex给其他线程使用
threadPool::Task threadPool::take(){
    Task task = nullptr;
    pthread_mutex_lock(&mutex_);
    while(taskQueue_.empty() && isRunning_){
        pthread_cond_wait(&condition_, &mutex_);
    }

    if(!isRunning_){
        pthread_mutex_unlock(&mutex_);
        return task;
    }
    assert(!taskQueue_.empty());

    task = taskQueue_.front();
    taskQueue_.pop_front();

    pthread_mutex_unlock(&mutex_);
    return task;
}

//线程执行的函数
void* threadPool::threadFunc(void *arg){
    pthread_t pid = pthread_self();
    threadPool* pool = static_cast<threadPool*>(arg);

    while( pool -> isRunning_){
        threadPool::Task task = pool->take();
        if(!task){
            printf("thread %lu will exit\n" , pid);
            break;
        }

        assert(task);
        task(); ///调用函数对象,可以通过函数对象获取返回值
        //printf("res : %d,",i);
    }
    return 0;
}
