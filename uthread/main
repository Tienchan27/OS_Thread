#include "uthread.h"
#include <iostream>

void thread_func1() {
    for (int i = 0; i < 3; ++i) {
        std::cout << "[Thread 1] Iteration " << i << std::endl;
        uthread_yield();
    }
    std::cout << "[Thread 1] Finished
";
    uthread_exit();
}

void thread_func2() {
    for (int i = 0; i < 2; ++i) {
        std::cout << "[Thread 2] Iteration " << i << std::endl;
        uthread_yield();
    }
    std::cout << "[Thread 2] Finished
";
    uthread_exit();
}

int main() {
    std::cout << "[Main] Creating user-level threads...
";
    uthread_create(thread_func1);
    uthread_create(thread_func2);
    uthread_run();
    std::cout << "[Main] All threads completed.
";
    return 0;
}
