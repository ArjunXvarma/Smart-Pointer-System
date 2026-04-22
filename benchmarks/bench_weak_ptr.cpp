#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <vector>
#include "../include/shared_ptr.hpp"
#include "../include/weak_ptr.hpp"
#include "utils.hpp"

#define benchmark benchmark_multi_thread

volatile int sink = 0;

constexpr int N = 1'000'000;
constexpr int THREADS = 8;
constexpr int THREAD_ITERS = 100'000;

void bench_std_lock_success() {
    std::shared_ptr<int> sp(new int(42));
    std::weak_ptr<int> wp(sp);

    for (int i = 0; i < N; ++i) {
        auto s = wp.lock();
        if (s) sink += *s;
    }
}

void bench_custom_lock_success() {
    sp::shared_ptr<int> sptr(new int(42));
    sp::weak_ptr<int> wp(sptr);

    for (int i = 0; i < N; ++i) {
        auto s = wp.lock();
        if (s) sink += *s;
    }
}

void bench_std_lock_fail() {
    std::weak_ptr<int> wp;

    {
        std::shared_ptr<int> sp(new int(42));
        wp = sp;
    }

    for (int i = 0; i < N; ++i) {
        auto s = wp.lock();
        if (!s) sink += 1;
    }
}

void bench_custom_lock_fail() {
    sp::weak_ptr<int> wp;

    {
        sp::shared_ptr<int> sptr(new int(42));
        wp = sptr;
    }

    for (int i = 0; i < N; ++i) {
        auto s = wp.lock();
        if (!s) sink += 1;
    }
}

void bench_std_construct() {
    std::shared_ptr<int> sp(new int(42));

    for (int i = 0; i < N; ++i) {
        std::weak_ptr<int> wp(sp);
    }
}

void bench_custom_construct() {
    sp::shared_ptr<int> sptr(new int(42));

    for (int i = 0; i < N; ++i) {
        sp::weak_ptr<int> wp(sptr);
    }
}

void bench_std_threaded() {
    std::shared_ptr<int> sp(new int(42));
    std::weak_ptr<int> wp(sp);

    std::vector<std::thread> threads;

    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([wp]() mutable {
            for (int i = 0; i < THREAD_ITERS; ++i) {
                auto s = wp.lock();
                if (s) sink += *s;
            }
        });
    }

    for (auto& th : threads) th.join();
}

void bench_custom_threaded() {
    sp::shared_ptr<int> sptr(new int(42));
    sp::weak_ptr<int> wp(sptr);

    std::vector<std::thread> threads;

    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([wp]() mutable {
            for (int i = 0; i < THREAD_ITERS; ++i) {
                auto s = wp.lock();
                if (s) sink += *s;
            }
        });
    }

    for (auto& th : threads) th.join();
}

int main() {
    std::cout << "=== weak_ptr Benchmark ===\n\n";

    std::cout << "--- lock() success ---\n";
    benchmark("std::weak_ptr lock success", bench_std_lock_success, N);
    benchmark("custom weak_ptr lock success", bench_custom_lock_success, N);

    std::cout << "\n--- lock() failure ---\n";
    benchmark("std::weak_ptr lock fail", bench_std_lock_fail, N);
    benchmark("custom weak_ptr lock fail", bench_custom_lock_fail, N);

    std::cout << "\n--- construction ---\n";
    benchmark("std::weak_ptr construct", bench_std_construct, N);
    benchmark("custom weak_ptr construct", bench_custom_construct, N);

    std::cout << "\n--- multithreaded ---\n";
    benchmark("std::weak_ptr threaded", bench_std_threaded, THREADS * THREAD_ITERS);
    benchmark("custom weak_ptr threaded", bench_custom_threaded, THREADS * THREAD_ITERS);

    std::cout << "\nIgnore: " << sink << "\n";
    return 0;
}