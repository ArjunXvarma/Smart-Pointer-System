//
// Created by Arjun Varma on 20/04/26.
//

#ifndef CONTROL_BLOCK_HPP
#define CONTROL_BLOCK_HPP

#include <cstddef>

struct control_block_base {
    size_t strong_count = 1;
    size_t weak_count = 0;

    virtual void destroy_object() noexcept = 0;
    virtual void delete_self() noexcept = 0;

    virtual ~control_block_base() = default;

    void add_strong() {
        ++strong_count;
    }

    void release_strong() {
        if (--strong_count == 0) {
            destroy_object();

            if (weak_count == 0) delete_self();
        }
    }

    void add_weak() {
        ++weak_count;
    }

    void release_weak() {
        if (--weak_count == 0 && strong_count == 0) delete_self();
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
    inplace_control_block(Args&... args) {
        new (storage) T(std::forward<Args>(args)...); // Allocate object T inside storage
    }

    T* get() {
        return reinterpret_cast<T*>(&storage); // Low level cast, convert raw memory to type T*
    }

    void destroy_object() noexcept override {
        get()->~T(); // Manual destructor call
    }

    void delete_self() noexcept override {
        operator delete(this); // Performs raw deletion of memory without calling destructor
    }
};

#endif //CONTROL_BLOCK_HPP