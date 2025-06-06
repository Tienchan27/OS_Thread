#include <windows.h>
#include <iostream>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

struct Task {
    int start;
    int end;
};

std::queue<Task> taskQueue;
std::mutex mtx;
std::condition_variable cv;
bool allTasksAssigned = false;

std::vector<unsigned long long> partialResults;

DWORD WINAPI ThreadFunction(LPVOID lpParam) {
    int threadID = *((int*)lpParam);

    while (true) {
        Task task;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [] { return !taskQueue.empty() || allTasksAssigned; });

            if (taskQueue.empty() && allTasksAssigned) {
                break;
            }

            task = taskQueue.front();
            taskQueue.pop();
            std::cout << "[START] LWP " << threadID << " starts doing task [" << task.start << ", " << task.end << "]\n";
        }

        unsigned long long result = 0;
        for (int i = task.start; i <= task.end; ++i) {
            result += i;
        }
        Sleep(1000 * (1 + rand() % 2)); //Giả lập xử lý nặng: random 1-2s 

        {
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "[FINISHED] LWP " << threadID << " is done with task [" << task.start << ", " << task.end << "] = " << result << "\n";
            partialResults.push_back(result);
        }
    }

    return 0;
}

int main() {
    std::cout << "========= LIGHTWEIGHT PROCESSES DEMONSTRATION =========\n";
    while (true) {
        // Xóa dữ liệu cũ
        while (!taskQueue.empty()) taskQueue.pop();
        partialResults.clear();
        allTasksAssigned = false;

        int N, maxPerThread, numThreads;
        std::cout << "------------------------------------------\n";
        std::cout << "Enter number of threads to use: ";
        std::cin >> numThreads;

        std::cout << "Enter number to compute sum: ";
        std::cin >> N;

        std::cout << "Enter max range each thread should handle per task: ";
        std::cin >> maxPerThread;

        std::cout << "------------------------------------------\n";
        std::cout << "================START DEMO================\n";
        //Chia các task thành từng đoạn [start, end]
        for (int i = 1; i <= N; i += maxPerThread) {
            int start = i;
            int end = std::min(i + maxPerThread - 1, N);
            taskQueue.push({ start, end });
        }

        int taskCount = taskQueue.size();
        numThreads = std::min(numThreads, taskCount);

        std::vector<HANDLE> handles(numThreads);
        std::vector<int> ids(numThreads);

        //Tạo các luồng
        for (int i = 0; i < numThreads; ++i) {
            ids[i] = i + 1;
            handles[i] = CreateThread(nullptr, 0, ThreadFunction, &ids[i], 0, nullptr);
        }

        {
            std::lock_guard<std::mutex> lock(mtx);
            allTasksAssigned = true;
        }
        cv.notify_all(); //Tbao cho tất cả các luồng là có task mới

        //Chờ tất cả các luồng hoàn thành
        WaitForMultipleObjects(numThreads, handles.data(), TRUE, INFINITE);

        //Đóng các handle của luồng
        for (HANDLE h : handles) {
            CloseHandle(h);
        }

        //Tổng hợp kết quả
        unsigned long long finalResult = 0;
        for (auto part : partialResults) {
            finalResult += part;
        }

        std::cout << "---------------------------------------------------\n";
        std::cout << "Final Result: 1 + 2 + ... + " << N << " = " << finalResult << "\n";

        char cont;
        std::cout << "------------------------------------------";
        std::cout << "\nDo you want to continue? (y/n): ";
        std::cin >> cont;
        if (cont != 'y' && cont != 'Y') break;
        std::cout << "\n";
    }
    return 0;
}
