#include <iostream>
#include <chrono>
#include <random>
#include "THREADPOOL.h"

void demoTask(int taskNum)
{
    auto threadID = std::this_thread::get_id();
    std::cout << "[START] Task " << taskNum + 1 << " is being processed by thread " << threadID << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1 + std::rand() % 5)); //Giả lập làm task từ 1-5s
    std::cout << "[DONE] Task " << taskNum + 1 << " finished on thread " << threadID << std::endl;
}

int main()
{
    std::cout << "=========THREAD POOL DEMOSTRATION=========" << std::endl;
    std::cout << "------------------------------------------" << std::endl;

    int numThread;
    std::cout << "Input number of threads in Thread Pool: ";
    std::cin >> numThread;
    int numTask;
    std::cout << "Input number of tasks in task queue: ";
    std::cin >> numTask;

    std::cout << "------------------------------------------" << std::endl;
    std::cout << "================START DEMO================" << std::endl;

    ThreadPool threads(numThread);
    for (int i = 0; i < numTask; i++)
    {
        threads.enqueue(demoTask, i);
        std::this_thread::sleep_for(std::chrono::milliseconds(300)); //delay giữa mỗi lần giao task nhìn cho dễ
    }

    std::this_thread::sleep_for(std::chrono::milliseconds( ( (int)std::ceil(numTask / (double) numThread) ) * 3000) );
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "Demo complete. Shutting down thread pool.\n";

    return 0;
}