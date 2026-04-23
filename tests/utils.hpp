struct Counter {
    static int constructions;
    static int destructions;

    Counter() { constructions++; }
    ~Counter() { destructions++; }
};

int Counter::constructions = 0;
int Counter::destructions = 0;

struct TrackingDeleter {
    int* counter;

    TrackingDeleter(int* c = nullptr) : counter(c) {}

    void operator()(int* p) {
        if (counter) (*counter)++;
        delete p;
    }
};

template<typename T>
struct CountingAllocator {
    using value_type = T;

    static int allocs;
    static int deallocs;

    CountingAllocator() = default;

    template<typename U>
    CountingAllocator(const CountingAllocator<U>&) {}

    T* allocate(size_t n) {
        allocs++;
        return std::allocator<T>{}.allocate(n);
    }

    void deallocate(T* p, size_t n) {
        deallocs++;
        std::allocator<T>{}.deallocate(p, n);
    }
};

template<typename T> int CountingAllocator<T>::allocs = 0;
template<typename T> int CountingAllocator<T>::deallocs = 0;

#define RUN_TEST(test) \
    std::cout << "[RUN] " << #test << "\n"; \
    test(); \
    std::cout << "[PASS] " << #test<< "\n\n";
