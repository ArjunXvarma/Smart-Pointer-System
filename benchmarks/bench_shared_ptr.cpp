#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <vector>

#include "utils.hpp"
#include "../include/make_shared.hpp"

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

int main() {
    std::cout << "=== shared_ptr Benchmark ===\n\n";

    std::cout << "--- Allocation ---\n";
    benchmark("std::shared_ptr alloc", bench_std_alloc, N);
    benchmark("custom shared_ptr alloc", bench_custom_alloc, N);
    benchmark("custom make_shared shared_ptr alloc", bench_make_shared, 1);

    std::cout << "\n--- Copy (atomic inc/dec) ---\n";
    benchmark("std::shared_ptr copy", bench_std_copy, N);
    benchmark("custom shared_ptr copy", bench_custom_copy, N);

    std::cout << "\n--- Move ---\n";
    benchmark("std::shared_ptr move", bench_std_move, N);
    benchmark("custom shared_ptr move", bench_custom_move, N);

    std::cout << "\n--- Reset ---\n";
    benchmark("std::shared_ptr reset", bench_std_reset, N);
    benchmark("custom shared_ptr reset", bench_custom_reset, N);

    std::cout << "\n--- Multithreaded Contention ---\n";
    benchmark("std::shared_ptr threaded", bench_std_threaded, THREADS * THREAD_ITERS);
    benchmark("custom shared_ptr threaded", bench_custom_threaded, THREADS * THREAD_ITERS);

    std::cout << "\nIgnore: " << sink << "\n";
    return 0;
}