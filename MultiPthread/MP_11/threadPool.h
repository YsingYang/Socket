#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <queue>
#include <type_traits>
#include <utility>
#include <vector>

class threadsGuard{
public:
    threadsGuard(std::vector<std::thread>& v) : threads_(v){

    }

    ~threadsGuard(){ ///对线程池中的线程进行析构
        for(size_t i = 0; i != threads_.size(); ++i){
            if(threads_[i].joinable()){
                threads_[i].join();
            }
        }
    }

private:
    threadsGuard(threadsGuard&& tg) = delete;///移动构造
    threadsGuard& operator = (threadsGuard &&tg) = delete; ///移动赋值

    threadsGuard(const threadsGuard&) = delete;
    threadsGuard& operator = (const threadsGuard&) = delete;


private:
    std::vector<std::thread>& threads_;
};

class threadPool{
public:
    typedef std::function<void()> task_type; ///任务类型别名

public:

    explicit threadPool(int n = 0);

    ~threadPool(){ //析构
        stop();
        condition_.notify_all();///条件变量通知所有释放
    }

    void stop(){
        stop_.store(true, std::memory_order_release);
    }

    template<class Function, class... Args>
    std::future<typename std::result_of<Function(Args...)>::type> add(Function&&, Args&&...);
private:
    std::atomic<bool> stop_; //原子操作 , 是否已经关闭
    std::mutex mutex_; //互斥量
    std::condition_variable condition_;//条件变量

    std::queue<task_type> tasks_;
    std::vector<std::thread> threads_;
    threadsGuard tg_;//这个到底有什么用

};

threadPool::threadPool(int n ) : stop_(false), tg_(threads_){
    int nthreads = n;
    if(nthreads <= 0){
        nthreads = std::thread::hardware_concurrency(); ///Returns the number of hardware thread contexts.
        nthreads = (nthreads == 0 ? 2 : nthreads);
    }
        std::cout<<"hardware_concurrency " << nthreads <<std::endl;
        ///返回当前计算机最大并发数量.
    for(int i = 0; i != nthreads; ++i){
        threads_.push_back(std::thread([this]{
                                while( !stop_.load(std::memory_order_acquire)){//从内存载入stop变量
                                            task_type task;
                                            {
                                                std::unique_lock<std::mutex> ulk(this->mutex_); ///unique_lock为管理mutex的对象
                                                this ->condition_.wait(ulk, [this]{
                                                                        return stop_.load(std::memory_order_acquire) || !this -> tasks_.empty();
                                                                       }); ///锁于条件变量, 直到stop为true, 或者tasks_队列不为空
                                                if(stop_.load(std::memory_order_acquire))
                                                    return ;
                                                task = std::move(this->tasks_.front());//取第一个任务
                                                this->tasks_.pop();
                                            }
                                            task();
                                       }
                           }));
    }
}

/***
add 操作返回一个future<Type> 类型,使得我们对于每个task get到相应的future返回给主线程,主线程捕获后移动至future的容器中,
从而读取到相应的返回值

***/
    template<class Function, class... Args>
    std::future<typename std::result_of<Function(Args...)>::type>  threadPool::add(Function &&fcn, Args&&...args){ ///为什么要传入右值引用
        typedef typename std::result_of<Function(Args...)>::type return_type;
        /// result_of  -> Obtains the result type of a call to Fn with arguments of the types listed in ArgTypes.
        typedef std::packaged_task<return_type()> task;
        ///A packaged_task wraps a callable element and allows its result to be retrieved asynchronously.
        auto t = std::make_shared<task>(std::bind(std::forward<Function>(fcn), std::forward<Args>(args)...)); ///注意args参数参数与使用是不同的
        auto ret = t->get_future();
        {
            std::lock_guard<std::mutex> lg(this->mutex_);
            if(stop_.load(std::memory_order_acquire))
                throw std::runtime_error("thread pool has stopped");
            tasks_.emplace([t]{
                        (*t)();
                        }); ///捕获只能指针,将函数对象传入队列中

        }
        condition_.notify_one();///通知一个线程
        return ret;
    }


#endif // THREADPOOL_H
