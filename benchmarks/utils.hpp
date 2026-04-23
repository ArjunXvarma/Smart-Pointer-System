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

struct CountingAllocatorStats {
    static inline int allocs = 0;
    static inline int deallocs = 0;
};

template<typename T>
struct CountingAllocator {
    using value_type = T;

    CountingAllocator() = default;

    // REQUIRED for rebind
    template<typename U>
    CountingAllocator(const CountingAllocator<U>&) {}

    T* allocate(std::size_t n) {
        CountingAllocatorStats::allocs++;
        return std::allocator<T>{}.allocate(n);
    }

    void deallocate(T* p, std::size_t n) {
        CountingAllocatorStats::deallocs++;
        std::allocator<T>{}.deallocate(p, n);
    }
};