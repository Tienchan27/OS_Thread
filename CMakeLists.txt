cmake_minimum_required(VERSION 3.10)
project(OS_Thread)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ULT demo
add_executable(uthread_demo
    uthread/main.cpp
    uthread/uthread.cpp
)

# Thread Pool demo
add_executable(thread_pool_demo
    DemoThreadPool/main.cpp
)

# LWP demo (Windows only)
if(WIN32)
    add_executable(lwp_demo
        DemoLightweightProcesses/LWP.cpp
    )
endif()

# Benchmark
add_executable(benchmark_demo
    benchmark.cpp
)
target_include_directories(benchmark_demo PRIVATE DemoThreadPool)
