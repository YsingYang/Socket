#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <deque>
#include <pthread.h>
#include <functional>


/**
template< class R, class... Args >
class function<R(Args...)>
**/
class threadPool{

public:
    typedef std::function<int()> Task; ///function模板参数

public:
    threadPool(int threadNum = 10);
    ~threadPool();

    size_t addTask(const Task& task);
    void stop();
    int size();
    Task take();

private:
    int createThreads();
    static void* threadFunc(void* threadData);

private:
    threadPool& operator = (threadPool&) = delete;
    threadPool(const threadPool&) = delete;

private:
    volatile bool isRunning_;
    int threadsNum_;
    pthread_t* threads_;  ///将这个改为vector存储 ?

     std::deque<Task> taskQueue_;
     pthread_mutex_t mutex_;
     pthread_cond_t condition_;

};



#endif // THREADPOOL_H
