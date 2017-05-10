#include <iostream>
#include <string>
#include "/home/ysing/codeblock Projects/Socket/MultiPthread/MP_11/threadPool.h"

int main(){
    std::mutex mtx;
    try{
        threadPool tp;
        std::vector<std::future<int>> v;
        std::vector<std::future<void>> v1;
        for(int i = 0 ; i <= 10; ++i){
            auto ans = tp.add([](int answer){
                                    return answer;
                              } , i); ///传入一个lambda ,第一个为函数对象, 第二个为参数
           v.push_back(std::move(ans));
        }
        for(int i = 0 ; i <= 5 ; ++i){
            auto ans = tp.add([&mtx](const std::string &s1, const std::string &s2){
                                std::lock_guard<std::mutex> lg(mtx);///保证函数退出时释放锁
                                std::cout<<(s1 + s2)<<std::endl;
                                return ;
                              }, "hello", "world");
                              v1.push_back(std::move(ans));
        }

        for(size_t i = 0 ; i < v.size() ; ++i){
            std::lock_guard<std::mutex> lg(mtx);
            std::cout<< v[i].get() <<std::endl;///异步,获取到返回值
        }
        for(size_t i = 0 ;i < v1.size(); ++i){
            v1[i].get();
        }
    }
    catch(std::exception&e){
            printf("exception \n");
            std::cout<<e.what()<<std::endl;
    }

}
