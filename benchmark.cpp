#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <ucontext.h>

using namespace std;
using namespace std::chrono;

// ---------- ULT Context Switch ----------
void ult_context_switch_test() {
    ucontext_t ctx1, ctx2;
    char stack1[1024 * 64], stack2[1024 * 64];

    getcontext(&ctx1);
    ctx1.uc_stack.ss_sp = stack1;
    ctx1.uc_stack.ss_size = sizeof(stack1);
    ctx1.uc_link = &ctx2;
    makecontext(&ctx1, [] {}, 0);

    getcontext(&ctx2);
    ctx2.uc_stack.ss_sp = stack2;
    ctx2.uc_stack.ss_size = sizeof(stack2);
    ctx2.uc_link = &ctx1;
    makecontext(&ctx2, [] {}, 0);

    const int iterations = 10000;
    auto start = high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        swapcontext(&ctx1, &ctx2);
        swapcontext(&ctx2, &ctx1);
    }
    auto end = high_resolution_clock::now();
    double avg = duration<double, micro>(end - start).count() / (iterations * 2);
    cout << "[ULT]         Avg context switch time: " << avg << " µs" << endl;
}

// ---------- Thread Pool ----------
#include "../thread_pool/ThreadPool.h"

void threadpool_switch_test() {
    ThreadPool pool(4);
    const int iterations = 1000;
    vector<future<long long>> results;

    auto start = high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        auto enqueue_time = high_resolution_clock::now();
        results.push_back(pool.enqueue([enqueue_time]() {
            auto exec_time = high_resolution_clock::now();
            return duration_cast<microseconds>(exec_time - enqueue_time).count();
        }));
    }

    long long total_latency = 0;
    for (auto& f : results) {
        total_latency += f.get();
    }

    double avg_latency = static_cast<double>(total_latency) / iterations;
    cout << "[ThreadPool]  Avg task latency: " << avg_latency << " µs" << endl;
}

// ---------- LWP Context Switch ----------
void lwp_context_switch_test() {
    const int iterations = 1000;
    atomic<bool> ready1(false), ready2(false);
    mutex mtx;
    condition_variable cv;
    long long total_time = 0;

    thread t1([&]() {
        for (int i = 0; i < iterations; ++i) {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [&] { return ready1.load(); });
            ready1.store(false);
            ready2.store(true);
            lock.unlock();
            cv.notify_one();
        }
    });

    thread t2([&]() {
        for (int i = 0; i < iterations; ++i) {
            auto start = high_resolution_clock::now();
            {
                unique_lock<mutex> lock(mtx);
                ready1.store(true);
                cv.notify_one();
                cv.wait(lock, [&] { return ready2.load(); });
                ready2.store(false);
            }
            auto end = high_resolution_clock::now();
            total_time += duration_cast<microseconds>(end - start).count();
        }
    });

    t1.join();
    t2.join();

    double avg = static_cast<double>(total_time) / iterations;
    cout << "[LWP]         Avg thread switch latency: " << avg << " µs" << endl;
}

// ---------- Main ----------
int main() {
    cout << "Benchmarking context switching performance..." << endl;
    ult_context_switch_test();
    threadpool_switch_test();
    lwp_context_switch_test();
    return 0;
}
