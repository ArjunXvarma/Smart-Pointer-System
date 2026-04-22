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

#define RUN_TEST(test) \
    std::cout << "[RUN] " << #test << "\n"; \
    test(); \
    std::cout << "[PASS] " << #test<< "\n\n";
