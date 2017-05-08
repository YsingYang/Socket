#ifndef THREADPOOL_H
#define THREADPOOL_H

#include<pthread.h>
#include<string.h>
#include <stdlib.h>
#include<deque>
#include <string>


class Task{
public:
    Task(void *arg = nullptr, const std::string taskName = ""):
        arg_(arg), taskName_(taskName){}

    virtual ~Task(){}

    void setArg(void *arg){
        arg_ = arg;
    }

    virtual int run() = 0;
    void* arg_;
    std::string taskName_;
private:
};

class threadPool{
public:
    threadPool(int threadNum = 10);
    ~threadPool();

public:
    size_t addTask(Task *task);
    size_t size();
    Task* take();
    void stop();
private:
    int createThreads();
    static void* threadFunc(void* threadData);

private:
    threadPool& operator=(const threadPool&) = delete;
    threadPool(const threadPool&) = delete;

private:
    volatile bool isRunning_; ///告诉编译器不用优化，每次访问都读取变量地址的值，而不是寄存器
    int threadsNum_;
    pthread_t * thread_;
    pthread_mutex_t mutex_;
    pthread_cond_t condition_ ; ///条件变量
    std::deque<Task*> taskQueue_ ; ///任务队列



};


#endif // THREADPOOL_H
