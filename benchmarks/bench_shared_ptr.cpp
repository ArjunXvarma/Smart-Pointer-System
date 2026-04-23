#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <vector>

#include "utils.hpp"
#include "../include/make_shared.hpp"
#include "../include/allocate_shared.hpp"

#define benchmark benchmark_multi_thread

volatile int sink = 0;

constexpr int N = 1'000'000;
constexpr int THREADS = 8;
constexpr int THREAD_ITERS = 100'000;

void bench_std_alloc() {
    for (int i = 0; i < N; ++i) {
        std::shared_ptr<int> p(new int(i));
        sink += *p;
    }
}

void bench_custom_alloc() {
    for (int i = 0; i < N; ++i) {
        sp::shared_ptr<int> p(new int(i));
        sink += *p;
    }
}

void bench_std_copy() {
    std::shared_ptr<int> base(new int(42));

    for (int i = 0; i < N; ++i) {
        std::shared_ptr<int> p = base;
        sink += *p;
    }
}

void bench_custom_copy() {
    sp::shared_ptr<int> base(new int(42));

    for (int i = 0; i < N; ++i) {
        sp::shared_ptr<int> p = base;
        sink += *p;
    }
}

void bench_std_move() {
    std::shared_ptr<int> base(new int(42));

    for (int i = 0; i < N; ++i) {
        std::shared_ptr<int> temp = std::move(base);
        base = std::move(temp);
    }
}

void bench_custom_move() {
    sp::shared_ptr<int> base(new int(42));

    for (int i = 0; i < N; ++i) {
        sp::shared_ptr<int> temp = std::move(base);
        base = std::move(temp);
    }
}

void bench_std_reset() {
    std::shared_ptr<int> p;

    for (int i = 0; i < N; ++i) {
        p.reset(new int(i));
        sink += *p;
    }
}

void bench_custom_reset() {
    sp::shared_ptr<int> p;

    for (int i = 0; i < N; ++i) {
        p.reset(new int(i));
        sink += *p;
    }
}

void bench_std_threaded() {
    std::shared_ptr<int> base(new int(42));

    std::vector<std::thread> threads;

    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([base]() mutable {
            for (int i = 0; i < THREAD_ITERS; ++i) {
                std::shared_ptr<int> local = base;
                sink += *local;
            }
        });
    }

    for (auto& th : threads) th.join();
}

void bench_custom_threaded() {
    sp::shared_ptr<int> base(new int(42));

    std::vector<std::thread> threads;

    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([base]() mutable {
            for (int i = 0; i < THREAD_ITERS; ++i) {
                sp::shared_ptr<int> local = base;
                sink += *local;
            }
        });
    }

    for (auto& th : threads) th.join();
}

void bench_make_shared() {
    for (int i = 0; i < N; ++i) {
        auto p = sp::make_shared<int>(i);
        sink += *p;
    }
}

void bench_std_shared_ptr_alloc() {
    for (int i = 0; i < N; ++i) {
        std::shared_ptr<int> p(new int(i));
        sink += *p;
    }
}

void bench_std_make_shared() {
    for (int i = 0; i < N; ++i) {
        auto p = std::make_shared<int>(i);
        sink += *p;
    }
}

void bench_custom_shared_ptr_alloc() {
    for (int i = 0; i < N; ++i) {
        sp::shared_ptr<int> p(new int(i));
        sink += *p;
    }
}

void bench_custom_make_shared() {
    for (int i = 0; i < N; ++i) {
        auto p = sp::make_shared<int>(i);
        sink += *p;
    }
}

void bench_custom_allocate_shared() {
    std::allocator<int> alloc;

    for (int i = 0; i < N; ++i) {
        auto p = sp::allocate_shared<int>(alloc, i);
        sink += *p;
    }
}

void bench_allocate_shared_custom_allocator() {
    using Alloc = CountingAllocator<int>;

    CountingAllocatorStats::allocs = 0;
    CountingAllocatorStats::deallocs = 0;

    for (int i = 0; i < N; ++i) {
        auto p = sp::allocate_shared<int>(Alloc{}, i);
        sink += *p;
    }

    std::cout << "Alloc calls: " << CountingAllocatorStats::allocs << "\n";
    std::cout << "Dealloc calls: " << CountingAllocatorStats::deallocs << "\n";
}

void bench_destruction() {
    std::vector<sp::shared_ptr<int>> vec;
    vec.reserve(N);

    for (int i = 0; i < N; ++i) {
        vec.push_back(sp::make_shared<int>(i));
    }

    auto start = Clock::now();
    vec.clear();  // triggers destruction
    auto end = Clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "Destruction: " << duration << " us\n";
}

int main() {
    std::cout << "=== shared_ptr Benchmark ===\n\n";

    std::cout << "--- Allocation (2 alloc vs 1 alloc) ---\n";
    benchmark("std::shared_ptr (new)", bench_std_shared_ptr_alloc, N);
    benchmark("std::make_shared", bench_std_make_shared, N);
    benchmark("custom shared_ptr (new)", bench_custom_shared_ptr_alloc, N);
    benchmark("custom make_shared", bench_custom_make_shared, N);
    benchmark("custom allocate_shared", bench_custom_allocate_shared, N);

    std::cout << "\n--- Copy (atomic inc/dec) ---\n";
    benchmark("std::shared_ptr copy", bench_std_copy, N);
    benchmark("custom shared_ptr copy", bench_custom_copy, N);

    std::cout << "\n--- Move ---\n";
    benchmark("std::shared_ptr move", bench_std_move, N);
    benchmark("custom shared_ptr move", bench_custom_move, N);

    std::cout << "\n--- Reset ---\n";
    benchmark("std::shared_ptr reset", bench_std_reset, N);
    benchmark("custom shared_ptr reset", bench_custom_reset, N);

    std::cout << "\n--- Custom Allocator (allocate_shared) ---\n";
    benchmark("sp::allocated_shared with custom allocator", bench_allocate_shared_custom_allocator, N);

    std::cout << "\n--- Destruction ---\n";
    benchmark("sp::shared_ptr destruction benchmark", bench_destruction, N);

    std::cout << "\n--- Multithreaded Contention ---\n";
    benchmark("std::shared_ptr threaded", bench_std_threaded, THREADS * THREAD_ITERS);
    benchmark("custom shared_ptr threaded", bench_custom_threaded, THREADS * THREAD_ITERS);

    std::cout << "\nIgnore: " << sink << "\n";
    return 0;
}