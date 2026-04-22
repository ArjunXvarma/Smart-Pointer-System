#include <chrono>

using Clock = std::chrono::high_resolution_clock;

template<typename Func>
void benchmark(const std::string& name, Func&& func) {
    auto start = Clock::now();
    func();
    auto end = Clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << name << ": " << duration << " us\n";
}

template<typename Func>
void benchmark_multi_thread(const std::string& name, Func&& func, int N) {
    auto start = Clock::now();
    func();
    auto end = Clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    double ns_per_op = (duration * 1000.0) / N;

    std::cout << name 
              << ": " << duration << " us"
              << " (" << ns_per_op << " ns/op)\n";
}