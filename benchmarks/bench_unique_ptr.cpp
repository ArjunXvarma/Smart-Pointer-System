#include <iostream>
#include <memory>
#include <chrono>
#include "../include/unique_ptr.hpp"
#include "utils.hpp"

volatile int sink = 0;

constexpr int N = 1'000'000;

void bench_raw_new_delete() {
    for (int i = 0; i < N; i++) {
        int* p = new int(i);
        sink += *p;
        delete p;
    }
}

void bench_std_unique_ptr() {
    for (int i = 0; i < N; i++) {
        std::unique_ptr<int> p(new int(i));
        sink += *p;
    }
}

void bench_custom_unique_ptr() {
    for (int i = 0; i < N; i++) {
        sp::unique_ptr<int> p(new int(i));
        sink += *p;
    }
}

void bench_raw_move() {
    int* p = new int(42);

    for (int i = 0; i < N; i++) {
        int* temp = p;
        p = temp;
    }

    delete p;
}

void bench_std_move() {
    std::unique_ptr<int> p(new int(42));

    for (int i = 0; i < N; i++) {
        std::unique_ptr<int> temp = std::move(p);
        p = std::move(temp);
    }
}

void bench_custom_move() {
    sp::unique_ptr<int> p(new int(42));

    for (int i = 0; i < N; i++) {
        sp::unique_ptr<int> temp = std::move(p);
        p = std::move(temp);
    }
}

void bench_raw_reset() {
    int* p = new int(42);
    for (int i = 0; i < N; i++) {
        delete p;
        p = new int(i);
        sink += *p;
    }

    delete p;
}

void bench_std_reset() {
    std::unique_ptr<int> p;
    for (int i = 0; i < N; i++) {
        p.reset(new int(i));
        sink += *p;
    }
}

void bench_custom_reset() {
    sp::unique_ptr<int> p;
    for (int i = 0; i < N; i++) {
        p.reset(new int(i));
        sink += *p;
    }
}

int main() {
    std::cout << "Running benchmarks (N = " << N << ")\n\n";

    benchmark("Raw new/delete", bench_raw_new_delete);
    benchmark("std::unique_ptr alloc", bench_std_unique_ptr);
    benchmark("custom unique_ptr alloc", bench_custom_unique_ptr);

    std::cout << "\n--- Move ---\n";
    
    benchmark("raw pointer move", bench_raw_move);
    benchmark("std::unique_ptr move", bench_std_move);
    benchmark("custom unique_ptr move", bench_custom_move);

    std::cout << "\n--- Reset ---\n";
    
    benchmark("raw pointer reset", bench_raw_reset);
    benchmark("std::unique_ptr reset", bench_std_reset);
    benchmark("custom unique_ptr reset", bench_custom_reset);

    std::cout << "\nIgnore: " << sink << "\n"; // prevent optimization
    return 0;
}