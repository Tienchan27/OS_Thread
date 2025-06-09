# OS Thread Demo: ULT vs LWP vs ThreadPool

This project demonstrates:
- User-Level Threads (ULT) using ucontext
- Thread Pool (std::thread based)
- LWP (Windows, via CreateThread)
- Context switching performance benchmark

## Build

```bash
mkdir build
cd build
cmake ..
make
```

## Run

### User-Level Thread
```
./uthread_demo
```

### Benchmark
```
./benchmark_demo
```

## Folder Structure

- `uthread/`: ULT implementation and demo
- `benchmark/`: Benchmark context switching latency
