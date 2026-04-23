//
// Created by Arjun Varma on 20/04/26.
//

#ifndef CONTROL_BLOCK_HPP
#define CONTROL_BLOCK_HPP

#include <cstddef>
#include <atomic>
#include <utility>

struct control_block_base {
    std::atomic<size_t> strong_count = 1;
    std::atomic<size_t> weak_count = 0;

    virtual void destroy_object() noexcept = 0;
    virtual void delete_self() noexcept = 0;

    virtual ~control_block_base() = default;

    void add_strong() noexcept {
        strong_count.fetch_add(1, std::memory_order_relaxed);
    }

    void release_strong() noexcept {
        if (strong_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            destroy_object();
            if (weak_count.load(std::memory_order_acquire) == 0) delete_self();
        }
    }

    void add_weak() noexcept {
        weak_count.fetch_add(1, std::memory_order_relaxed);
    }

    void release_weak() noexcept {
        if (weak_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            if (strong_count.load(std::memory_order_acquire) == 0) delete_self();
        }
    }
};

template<typename T, typename Deleter>
struct separate_control_block : control_block_base {
    T* ptr;
    Deleter deleter;

    separate_control_block(T* p, Deleter d) : ptr(p), deleter(std::move(d)) {}

    void destroy_object() noexcept override {
        deleter(ptr);
    }

    void delete_self() noexcept override {
        delete this;
    }
};

template<typename T>
struct inplace_control_block : control_block_base {
    alignas(T) unsigned char storage[sizeof(T)];

    template<typename... Args>
    inplace_control_block(Args&&... args) {
        new (storage) T(std::forward<Args>(args)...); // Allocate object T inside storage
    }

    T* get() {
        return reinterpret_cast<T*>(&storage); // Low level cast, convert raw memory to type T*
    }

    void destroy_object() noexcept override {
        get()->~T(); // Manual destructor call
    }

    void delete_self() noexcept override {
        // Using operator delete is DIFFERENT from delete, operator delete deallocates memory without calling destructor
        operator delete(this); // Performs raw deletion of memory without calling destructor
    }
};

template<typename T, typename Alloc>
struct inplace_control_block_alloc : control_block_base {
    using AllocTraits = std::allocator_traits<Alloc>;

    alignas(T) unsigned char storage[sizeof(T)];
    Alloc alloc;

    template<typename... Args>
    inplace_control_block_alloc(const Alloc& a, Args&&... args) : alloc(a) {
        new (storage) T(std::forward<Args>(args)...); // Allocate object T inside storage
    }

    T* get() {
        return reinterpret_cast<T*>(&storage); // Low level cast, convert raw memory to type T*
    }

    void destroy_object() noexcept override {
        get()->~T(); // Manual destructor call
    }

    void delete_self() noexcept override {
        using CBAlloc = AllocTraits::template rebind_alloc<inplace_control_block_alloc>;

        CBAlloc cb_alloc(alloc);

        std::allocator_traits<CBAlloc>::deallocate(cb_alloc, this, 1);
    }
};

#endif //CONTROL_BLOCK_HPP