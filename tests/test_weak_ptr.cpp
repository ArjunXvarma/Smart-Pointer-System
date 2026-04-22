#include <iostream>
#include <cassert>
#include <thread>
#include <vector>
#include "../include/shared_ptr.hpp"
#include "../include/weak_ptr.hpp"
#include "utils.hpp"

volatile int sink = 0;

void test_basic() {
    sp::shared_ptr<int> sp(new int(42));
    sp::weak_ptr<int> wp(sp);

    assert(!wp.expired());
    assert(wp.use_count() == 1);
}

void test_lock_success() {
    sp::shared_ptr<int> sp(new int(42));
    sp::weak_ptr<int> wp(sp);

    auto sp2 = wp.lock();

    assert(sp2.get() != nullptr);
    assert(*sp2 == 42);
    // shared_ptr copy assignment operator called
    assert(sp2.use_count() == 2);
}

void test_lock_expired() {
    sp::weak_ptr<int> wp;

    {
        sp::shared_ptr<int> sp(new int(42));
        wp = sp;
    }

    assert(wp.expired());

    auto sp2 = wp.lock();
    assert(sp2.get() == nullptr);
}

void test_copy() {
    sp::shared_ptr<int> sp(new int(42));
    sp::weak_ptr<int> w1(sp);
    sp::weak_ptr<int> w2 = w1;

    assert(!w1.expired());
    assert(!w2.expired());
    assert(w1.use_count() == 1);
}

void test_move() {
    sp::shared_ptr<int> sp(new int(42));
    sp::weak_ptr<int> w1(sp);
    sp::weak_ptr<int> w2(std::move(w1));

    assert(w1.expired() || w1.use_count() == 0);
    assert(!w2.expired());
}

void test_expiration() {
    sp::weak_ptr<int> wp;

    {
        sp::shared_ptr<int> sp(new int(42));
        wp = sp;
        assert(!wp.expired());
    }

    assert(wp.expired());
}

void test_destruction() {
    Counter::constructions = 0;
    Counter::destructions = 0;

    sp::weak_ptr<Counter> wp;

    {
        sp::shared_ptr<Counter> sp(new Counter());
        wp = sp;

        assert(Counter::constructions == 1);
        assert(Counter::destructions == 0);
    }

    // object should be destroyed even if sp::weak_ptr exists
    assert(Counter::destructions == 1);
}

void test_multiple_weak() {
    sp::shared_ptr<int> sp(new int(42));

    sp::weak_ptr<int> w1(sp);
    sp::weak_ptr<int> w2(sp);
    sp::weak_ptr<int> w3(sp);

    assert(w1.use_count() == 1);
    assert(!w2.expired());
    assert(!w3.expired());
}

void test_lock_scope() {
    sp::weak_ptr<int> wp;

    {
        sp::shared_ptr<int> sp(new int(42));
        wp = sp;

        {
            auto sp2 = wp.lock();
            assert(sp2.get() != nullptr);
        }

        assert(!wp.expired());
    }

    assert(wp.expired());
}

void test_stress_single_thread() {
    sp::shared_ptr<int> sp(new int(42));
    sp::weak_ptr<int> wp(sp);

    for (int i = 0; i < 100000; ++i) {
        sp::shared_ptr<int> temp = wp.lock();
        if (temp) 
            assert(*temp == 42);
    }
}

void test_thread_safety() {
    sp::shared_ptr<int> sp(new int(42));
    sp::weak_ptr<int> wp(sp);

    std::thread t1([&]() {
        for (int i = 0; i < 100000; ++i) {
            sp::shared_ptr<int> temp = wp.lock();
            if (temp) 
                sink += *temp;
        }
    });

    std::thread t2([&]() {
        sp.reset();
    });

    t1.join();
    t2.join();

    // Should not crash or use-after-free
    assert(true);
}

int main() {
    RUN_TEST(test_basic);
    RUN_TEST(test_lock_success);
    RUN_TEST(test_lock_expired);
    RUN_TEST(test_copy);
    RUN_TEST(test_move);
    RUN_TEST(test_expiration);
    RUN_TEST(test_destruction);
    RUN_TEST(test_multiple_weak);
    RUN_TEST(test_lock_scope);
    RUN_TEST(test_stress_single_thread);
    RUN_TEST(test_thread_safety);

    std::cout << "All sp::weak_ptr tests passed\n";
    return 0;
}