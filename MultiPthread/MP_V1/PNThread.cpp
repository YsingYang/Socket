#include "PNThread.h"

PNThread::PNThread():isFree(true) , task(nullptr){
    threadMember = std::thread(&PNThread::run,this); //为什么需要传入this指针?
    threadMember.detach();///挂在后台运行
}

//返回线程状态
inline bool PNThread::getStatus() const {
    return isFree;
}

//设置线程状态,true : 空闲 , false :忙碌
inline void PNThread::setStatus(bool status){
    isFree = status;
}

void PNThread::run(){
    //while(true){ //不自旋可以吗
        locker.lock();
        if(task && !getStatus()){//有task任务.执行task任务
            task->run();
            setStatus(true);//task 执行完,处于空闲状态,可以进行回收
        }
        locker.unlock();
    //}
}

bool PNThread::addTask(baseTask *_task){
    if(getStatus()&& locker.try_lock()){ //如果处于空闲,可加入
        task = _task;
        setStatus(false);//加入任务后,当前进程进入处于繁忙
        threadMember = std::thread(&PNThread::run,this);
        threadMember.detach();
        locker.unlock();
        return true;
    }
    return false;
}

PNThread::~PNThread(){

}
