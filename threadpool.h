#include<thread>
#include<condition_variable>
#include<mutex>
#include<vector>
#include<queue>
#include<future>

class ThreadPool{
private:
    bool m_stop;
    std::vector<std::thread>m_thread;
    std::queue<std::function<void()>>tasks;
    std::mutex m_mutex;
    std::condition_variable m_cv;

public:
    explicit ThreadPool(size_t threadNumber):m_stop(false){
        for(size_t i=0;i<threadNumber;++i)
        {
            m_thread.emplace_back(
                [this](){
                    for(;;)
                    {
                        std::function<void()>task;
                        {
                            std::unique_lock<std::mutex>lk(m_mutex);
                            m_cv.wait(lk,[this](){ return m_stop||!tasks.empty();});
                            if(m_stop&&tasks.empty()) return;
                            task=std::move(tasks.front());
                            tasks.pop();
                        }
                        task();
                    }
                }
            );
        }
    }

    ThreadPool(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;

    ThreadPool & operator=(const ThreadPool &) = delete;
    ThreadPool & operator=(ThreadPool &&) = delete;

    ~ThreadPool(){
        {
            std::unique_lock<std::mutex>lk(m_mutex);
            m_stop=true;
        }
        m_cv.notify_all();
        for(auto& threads:m_thread)
        {
            threads.join();
        }
    }

    template<typename F,typename... Args>
    auto submit(F&& f,Args&&... args)->std::future<decltype(f(args...))>{
        //make_shared<type>(初始化变量)
        auto taskPtr=std::make_shared< std::packaged_task<decltype(f(args...))()> >( 
            std::bind(std::forward<F>(f),std::forward<Args>(args)...) //forward 完美转发
        );
        {
            std::unique_lock<std::mutex>lk(m_mutex);
            if(m_stop) throw std::runtime_error("submit on stopped ThreadPool");
            //[捕获列表] 
            // Lambda 表达式内部函数体在默认情况下是不能够使用函数体外部的变量的， 
            // 这时候捕获列表可以起到传递外部数据的作用
            tasks.emplace([taskPtr](){ (*taskPtr)(); });
        }
        m_cv.notify_one();
        return taskPtr->get_future();

    }

};