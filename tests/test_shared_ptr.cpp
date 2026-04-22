#include <iostream>
#include <memory>
#include <cassert>
#include <thread>
#include <vector>

#include "../include/shared_ptr.hpp"
#include "utils.hpp"

using namespace sp;

void test_control_block() {
    using CB = separate_control_block<int, std::default_delete<int>>;

    CB* cb = new CB(new int(42), std::default_delete<int>());

    cb->add_strong();
    cb->release_strong();
    cb->release_strong();

    cb->add_weak();
    cb->release_weak();
}

void test_basic() {
    shared_ptr<int> p(new int(42));
    assert(p.get() != nullptr);
    assert(*p == 42);
    assert(p.use_count() == 1);
}

void test_copy_constructor() {
    shared_ptr<int> p1(new int(42));
    shared_ptr<int> p2(p1);

    assert(p1.get() == p2.get());
    assert(p1.use_count() == 2);
    assert(p2.use_count() == 2);
}

void test_copy_assignment() {
    shared_ptr<int> p1(new int(42));
    shared_ptr<int> p2;

    p2 = p1;
    assert(p1.use_count() == 2);
    assert(p2.use_count() == 2);
}

void test_move_constructor() {
    shared_ptr<int> p1(new int(42));
    shared_ptr<int> p2(std::move(p1));

    assert(p1.get() == nullptr);
    assert(p2.get() != nullptr);
    assert(p2.use_count() == 1);
}

void test_move_assignment() {
    shared_ptr<int> p1(new int(42));
    shared_ptr<int> p2;

    p2 = std::move(p1);
    assert(p1.get() == nullptr);
    assert(p2.use_count() == 1);
}

void test_self_assignment() {
    shared_ptr<int> p(new int(42));
    shared_ptr<int>& ref = p;
    p = ref;

    assert(p.use_count() == 1);
    assert(*p == 42);
}

void test_destruction() {
    Counter::constructions = 0;
    Counter::destructions = 0;

    {
        shared_ptr<Counter> p1(new Counter()); // strong_count = 1
        {
            shared_ptr<Counter> p2 = p1; // strong_count = 2
            assert(p1.use_count() == 2);
        } // strong_count = 1

        // strong_count = 1, so no destruction
        assert(Counter::destructions == 0);
    } // strong_count = 0, cb deleted

    assert(Counter::destructions == 1);
}

void test_reset() {
    shared_ptr<int> p(new int(42));
    p.reset(new int(24));

    assert(p.use_count() == 1);
    assert(*p == 24);
}

void test_custom_deleter() {
    int delete_count = 0;

    {
        shared_ptr<int> p(new int(42), TrackingDeleter(&delete_count));
    }

    assert(delete_count == 1);
}

void test_multiple_copies() {
    shared_ptr<int> p1(new int(42));
    shared_ptr<int> p2 = p1;
    shared_ptr<int> p3 = p2;

    assert(p1.use_count() == 3);
    assert(p2.use_count() == 3);
    assert(p3.use_count() == 3);
}

void test_reassignment_release() {
    Counter::constructions = 0;
    Counter::destructions = 0;

    shared_ptr<Counter> p(new Counter());
    p = shared_ptr<Counter>(new Counter());

    assert(Counter::destructions == 1);
}

void test_null_behaviour() {
    shared_ptr<int> p;

    assert(p.get() == nullptr);
    assert(p.use_count() == 0);

    p.reset();
    assert(p.get() == nullptr);
}

void test_scope_stress() {
    Counter::constructions = 0;
    Counter::destructions = 0;

    {
        shared_ptr<Counter> p(new Counter());
        for (int i = 0; i < 1000; i++) {
            shared_ptr<Counter> temp = p;
        }
    }

    assert(Counter::destructions == 1);
}

void test_thread_safety() {
    shared_ptr<int> p(new int(42));

    const int n_threads = 8;
    const int iterations = 100000;

    std::vector<std::thread> threads;

    for (int t = 0; t < n_threads; t++) {
        threads.emplace_back([p]() mutable {
            for (int i = 0; i < iterations; i++) {
                shared_ptr<int> local = p;
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    assert(p.use_count() == 1);
}

int main() {
    RUN_TEST(test_control_block);
    RUN_TEST(test_basic);
    RUN_TEST(test_copy_constructor);
    RUN_TEST(test_copy_assignment);
    RUN_TEST(test_move_constructor);
    RUN_TEST(test_move_assignment);
    RUN_TEST(test_self_assignment);
    RUN_TEST(test_destruction);
    RUN_TEST(test_reset);
    RUN_TEST(test_custom_deleter);
    RUN_TEST(test_multiple_copies);
    RUN_TEST(test_reassignment_release);
    RUN_TEST(test_null_behaviour);
    RUN_TEST(test_scope_stress);
    RUN_TEST(test_thread_safety);

    std::cout << "All shared_ptr tests passed\n";

    return 0;
}
























// meeting feedback
// work on chapter 1 introduction - read like abstract (summary), but more detail. atleast 3 quarter of the page
// mention that decoding part is more significant in LLMs
// Put dataset construction in chatper 2
// In 2.3 - talk about what requirements you're looking for in a model
// In 2.4 and 2.5 - justify the use of evaluation metrics.
// Mention AIRE and project management (pushes to github etc) in methodology
// Add a comprehensive example in the qualitative section
// 4.2.1 draw a relation between problems and impact in the ed text gen context
// Talk about ethics and project management
// In chapter 4, fix the discussion parts - talk about discussion of results
// Restructure future works such as idea 1, idea 2, etc