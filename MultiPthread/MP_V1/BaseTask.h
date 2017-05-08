#ifndef BASETASK_H
#define BASETASK_H
#include <iostream>
class baseTask{
public:
    baseTask(size_t _No = 0):taskNo(_No){}
    //删除copy constructor , copy assignment
    baseTask(baseTask &) = delete;
    baseTask operator = (baseTask &) = delete;

    virtual void run(){//Task的run方法,由继承类自己重写
            std::cout<<"My taskNo " <<taskNo<<std::endl;
    }

    virtual ~baseTask(){}

private:
    size_t taskNo;//Task的编号


};

#endif // BASETASK
