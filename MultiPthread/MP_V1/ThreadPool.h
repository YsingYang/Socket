#ifndef THREADPOOL_H
#define THREADPOOL_H


#include <mutex>
#include "BaseTask.h"
#include "PNThread.h"
#include <queue>
#include <vector>

const int MAXTASKWAITING = 10; //任务队列最大为10

class threadPool{
public:
    threadPool(int num = 10);
    threadPool(threadPool &) = delete;
    threadPool operator= (threadPool &) = delete;

    bool addTask(baseTask *);
    void run();

    virtual ~threadPool();


protected:

private:
    std::queue<baseTask *>taskQueue;

    std::vector<PNThread*>TPool;
    std::mutex locker;
    int freeThread;
};


#endif // THREADPOOL_H
