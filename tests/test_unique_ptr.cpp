#include <iostream>
#include <cassert>
#include <cstdio>
#include "../include/unique_ptr.hpp"

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

// Basic ownership
void test_basic_ownership() {
    sp::unique_ptr<int> p(new int(42));
    assert(p.get() != nullptr);
    assert(*p == 42);
}

// Move constructor
void test_move_constructor() {
    sp::unique_ptr<int> p1(new int(42));
    sp::unique_ptr<int> p2(std::move(p1));

    assert(p1.get() == nullptr);
    assert(p2.get() != nullptr);
    assert(*p2 == 42);
}

// Self move assignment
// void test_self_move_assignment() {
//     sp::unique_ptr<int> p(new int(42));

//     p = std::move(p);
//     assert(p.get() != nullptr);
//     assert(*p == 42);
// }

void test_destruction() {
    Counter::destructions = 0;
    Counter::constructions = 0;

    {
        sp::unique_ptr<Counter> p(new Counter());
        assert(Counter::constructions == 1);
        assert(Counter::destructions == 0);
    }

    assert(Counter::destructions == 1);
    assert(Counter::constructions == 1);
}

void test_reset() {
    sp::unique_ptr<int> p(new int(42));
    p.reset(new int(24));
    assert(*p == 24);
}

void test_release() {
    sp::unique_ptr<int> p1(new int(42));
    int* p2 = p1.release();
    assert(*p2 == 42);
    assert(p1.get() == nullptr);

    delete p2;
}

void test_custom_deleter() {
    int delete_count = 0;
    
    {
        sp::unique_ptr<int, TrackingDeleter> p(new int(42), TrackingDeleter(&delete_count)); 
    }

    assert(delete_count == 1);
}

void test_multiple_moves_chain() {
    sp::unique_ptr<int> p1(new int(42));
    sp::unique_ptr<int> p2(std::move(p1));
    sp::unique_ptr<int> p3(std::move(p2));

    assert(p1.get() == nullptr);
    assert(p2.get() == nullptr);
    assert(*p3 == 42);
}

// Size check (Empty Base optimisation)
void test_size() {
    struct empty_deleter {
        void operator()(int* p) { delete p; }
    };

    using UP = sp::unique_ptr<int, empty_deleter>;

    assert(sizeof(UP) == sizeof(int*));
}

int main() {
    RUN_TEST(test_basic_ownership);
    RUN_TEST(test_move_constructor);
    // RUN_TEST(test_self_move_assignment);
    RUN_TEST(test_destruction);
    RUN_TEST(test_reset);
    RUN_TEST(test_release);
    RUN_TEST(test_custom_deleter);
    RUN_TEST(test_multiple_moves_chain);
    RUN_TEST(test_size);

    std::cout << "All tests passed" << std::endl;
}
