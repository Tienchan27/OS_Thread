#include "uthread.h"
#include <ucontext.h>
#include <queue>
#include <cstdlib>
#include <iostream>

#define STACK_SIZE 1024 * 64

enum ThreadStatus { READY, RUNNING, FINISHED };

struct ThreadControlBlock {
    int tid;
    ucontext_t context;
    void* stack;
    ThreadStatus status;
};

std::queue<ThreadControlBlock*> readyQueue;
ThreadControlBlock* currentThread = nullptr;
int tid_counter = 0;

void uthread_create(void (*func)(void)) {
    ThreadControlBlock* tcb = new ThreadControlBlock();
    tcb->tid = ++tid_counter;
    tcb->stack = malloc(STACK_SIZE);
    getcontext(&tcb->context);
    tcb->context.uc_stack.ss_sp = tcb->stack;
    tcb->context.uc_stack.ss_size = STACK_SIZE;
    tcb->context.uc_link = nullptr;
    makecontext(&tcb->context, func, 0);
    tcb->status = READY;
    readyQueue.push(tcb);
}

void uthread_yield() {
    if (currentThread && currentThread->status == RUNNING) {
        currentThread->status = READY;
        readyQueue.push(currentThread);
    }
    ThreadControlBlock* next = readyQueue.front();
    readyQueue.pop();
    ThreadControlBlock* prev = currentThread;
    currentThread = next;
    currentThread->status = RUNNING;
    if (prev) {
        swapcontext(&prev->context, &currentThread->context);
    } else {
        setcontext(&currentThread->context);
    }
}

void uthread_exit() {
    currentThread->status = FINISHED;
    free(currentThread->stack);
    delete currentThread;
    currentThread = nullptr;
    if (!readyQueue.empty()) {
        ThreadControlBlock* next = readyQueue.front();
        readyQueue.pop();
        currentThread = next;
        currentThread->status = RUNNING;
        setcontext(&currentThread->context);
    } else {
        exit(0);
    }
}

void uthread_run() {
    if (!readyQueue.empty()) {
        currentThread = readyQueue.front();
        readyQueue.pop();
        currentThread->status = RUNNING;
        setcontext(&currentThread->context);
    }
}
