#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <iostream>
#include <thread>
#include <future>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool {
    private:
        std::mutex queueMutex;
        std::vector<std::thread> workerThreads;
        std::queue< std::function<void()> > tasks;
        std::condition_variable condition;
        bool stopTask;

    public:

    ThreadPool(int numThreads)
    {
        for (int i = 0; i < numThreads; i++){
            workerThreads.emplace_back(
                [this](){
                    while(true)
                    {
                        std::function<void()> task;
                        
                        {
                            std::unique_lock<std::mutex> lock(queueMutex); //Kiểm soát mutex
                            //Đợi được Thread nào đó gọi dậy -> nếu dừng task queue hoặc còn việc (task queue chưa empty) thì dậy
                            this->condition.wait(lock, [this](){return this->stopTask || !this->tasks.empty();}); 
                            //Dừng task queue và hết việc rồi thì thoát khỏi vòng lặp vô tận
                            if(this->stopTask && this->tasks.empty())
                                break;
                            //Lấy task vào cho thread làm
                            task = std::move(this->tasks.front());
                            this->tasks.pop();
                        }

                        //Do the task
                        task();
                    }
                }
            );
        }
    }

    

    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stopTask = true;
        }
        condition.notify_all();
        for (std::thread& worker : this->workerThreads)
            worker.join()
    }

    
    
};



#endif