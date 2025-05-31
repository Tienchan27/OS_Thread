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
        bool stop;

    public:

    ThreadPool(int numThreads) : stop(false)
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
                            this->condition.wait(lock, [this](){ return this->stop || !this->tasks.empty(); }); 

                            //Dừng task queue và hết việc rồi thì thoát khỏi vòng lặp vô tận
                            if(this->stop && this->tasks.empty())
                                return;

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

    
    //Thêm task vào pool
    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::invoke_result<F, Args...>::type> //Task sẽ đc future xử lý, tính toán và lưu kqua
    {
        //Kiểu dữ liệu trả về của function/task
        using funcReturnType = typename std::invoke_result<F, Args...>::type;

        //Dùng shared pointer trỏ tới packaged task, bind function F với arguments [tương đương gọi hàm F(args1, args2,...)]
        auto task = std::make_shared< std::packaged_task<funcReturnType()> > (
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        //Lấy kết quả mà task trả về
        std::future<funcReturnType> result = task->get_future();

        {
            std::unique_lock<std::mutex> lock(queueMutex);

            //Dừng hết
            if(stop)
                throw std::runtime_error("Thread Pool is closed.");

            tasks.emplace( [task](){ (*task)(); });
        }

        condition.notify_one();
        return result;
    }


    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : this->workerThreads)
            worker.join();
    }

    
};



#endif