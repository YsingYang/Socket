#ifndef PNTHREAD_H
#define PNTHREAD_H

#include <thread>
#include <mutex>
#include "BaseTask.h"
class PNThread{
public:
    PNThread();//默认构造函数
    PNThread(PNThread &) = delete;//删除copy constructor
    PNThread operator= (PNThread &) = delete; //删除copy assignment

    bool getStatus() const ;//获得当前线程状态
    void setStatus(bool);//设置当前线程状态
    bool addTask(baseTask *); //加入Task,需要加入返回值作为是否加入成功判断

    void run();

    virtual ~PNThread(); //析构函数

private:
    std::thread threadMember; //封装在线程类中的线程
    bool isFree; //当前线程是否在空闲状态,是否有Task
    baseTask *task; //当前所执行的任务
    std::mutex locker; //互斥锁
};

#endif // THREAD

