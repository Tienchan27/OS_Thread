cmake_minimum_required(VERSION 3.10)
project(OS_ThreadDemo_ContextSwitching)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(uthread_demo uthread/main.cpp uthread/uthread.cpp)
add_executable(benchmark_demo benchmark/benchmark.cpp)
