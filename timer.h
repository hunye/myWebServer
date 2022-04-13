#ifndef TIMER_H
#define TIMER_H

#include<queue>
#include<deque>
#include<unordered_map>
#include<ctime>
#include<chrono>
#include<functional>
#include<memory>


#include "HTTPconnection.h"

typedef std::function<void()> TimeoutCallBack;
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds MS;
typedef Clock::time_point TimeStamp;

class TimerNode{
public: 
    int id; //标记定时器
    TimeStamp expire;//过期时间
    TimeoutCallBack cb;//回调函数，方便删除定时器时将对应的HTTP连接关闭

    bool operator<(const TimerNode& t){
        return expire<t.expire;
    }
};
class TimerManager{
    typedef std::shared_ptr<TimerNode> SP_TimerNode;
public:
    TimerManager() {heap_.reserve(64);}
    ~TimerManager() {clear();}
    //设置定时器
    void addTimer(int id, int timeout, const TimeoutCallBack& cb);
    //处理过期的定时器
    void handle_expired_event();
    //下次处理过期定时器的时间
    int getNextHandle();

    void update(int id, int timeout);
    //删除id节点， 指针触发处理函数
    void work(int id);
    
    void pop();
    void clear();
private:
    void del_(size_t i);//删除指定定时器
    void siftup_(size_t i);//向上调整
    bool siftdown_(size_t index, size_t n);//向下调整
    void swapNode_(size_t i, size_t j);//交换两个结点位置

    std::vector<TimerNode>heap_;
    std::unordered_map<int, size_t> ref_;
};
#endif