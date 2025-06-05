#include <iostream>
#include <chrono>
#include <random>
#include "THREADPOOL.h"

void demoTask(int taskNum)
{
    auto threadID = std::this_thread::get_id();
    std::cout << "[START] Task " << taskNum + 1 << " is being processed by thread " << threadID << "\n";
    std::this_thread::sleep_for(std::chrono::seconds(1 + std::rand() % 3)); //Giả lập làm task từ 1-3s
    std::cout << "[DONE] Task " << taskNum + 1 << " finished on thread " << threadID << "\n";
}

int main()
{
    std::cout << "=========THREAD POOL DEMONSTRATION=========" << std::endl;

    while (true) {
        int numThread;
        std::cout << "------------------------------------------" << std::endl;
        std::cout << "Input number of threads in Thread Pool: ";
        std::cin >> numThread;
        int numTask;
        std::cout << "Input number of tasks in task queue: ";
        std::cin >> numTask;

        std::cout << "------------------------------------------" << std::endl;
        std::cout << "================START DEMO================" << std::endl;

        ThreadPool threads(numThread);

        //Tạo một vector để lưu trữ các future
        std::vector<std::future<void>> futures;
        for (int i = 0; i < numTask; i++)
        {
            futures.push_back(threads.enqueue(demoTask, i)); //Thêm task vào thread pool
            std::this_thread::sleep_for(std::chrono::milliseconds(300)); //Giả lập thời gian tạo task mới
        }

        //Đợi tất cả các task hoàn thành
        for (auto& fut : futures) {
            fut.get();
        }
        std::cout << "------------------------------------------" << std::endl;
        std::cout << "Demo complete. Shutting down thread pool.\n";

        char cont;
        std::cout << "------------------------------------------";
        std::cout << "\nDo you want to continue? (y/n): ";
        std::cin >> cont;
        if (cont != 'y' && cont != 'Y') break;
        std::cout << std::endl;
    }
    return 0;
}