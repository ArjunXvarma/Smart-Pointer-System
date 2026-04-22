#ifndef SHARED_PTR
#define SHARED_PTR

#include <memory>
#include "control_block.hpp"

namespace sp {
    template<typename T>
    class shared_ptr {
    private:
        T* ptr = nullptr;
        control_block_base* cb = nullptr;

    public:
        shared_ptr() noexcept = default;

        template<typename Deleter = std::default_delete<T>>
        explicit shared_ptr(T* p, Deleter d = Deleter()) {
            cb = new separate_control_block<T, Deleter>(p, std::move(d)); // creates control block, stores pointer, sets strong_count = 1
            ptr = p;
        }

        shared_ptr(const shared_ptr& other) noexcept {
            ptr = other.ptr;
            cb = other.cb;

            if (cb) cb->add_strong();
        }

        shared_ptr& operator=(const shared_ptr& other) {
            if (this != &other) {
                release();
                ptr = other.ptr;
                cb = other.cb;

                if (cb) cb->add_strong();
            }

            return *this;
        }

        shared_ptr(shared_ptr&& other) noexcept {
            ptr = other.ptr;
            cb = other.cb;

            other.ptr = nullptr;
            other.cb = nullptr;
        }

        shared_ptr& operator=(shared_ptr&& other) noexcept {
            if (this != &other) {
                release();
                ptr = other.ptr;
                cb = other.cb;

                other.ptr = nullptr;
                other.cb = nullptr;
            }

            return *this;
        }

        void release() {
            if (!cb) return;

            cb->release_strong();

            ptr = nullptr;
            cb = nullptr;
        }

        T* get() const noexcept {
            return ptr;
        }

        T& operator*() const {
            return *ptr;
        }

        T* operator->() const noexcept {
            return ptr;
        }

        size_t use_count() const noexcept {
            return cb ? cb->strong_count.load(std::memory_order_acquire) : 0;
        }

        void reset() {
            release();
        }

        template<typename Deleter = std::default_delete<T>>
        void reset(T* p, Deleter d = Deleter()) {
            release();
            cb = new separate_control_block<T, Deleter>(p, std::move(d));
            ptr = p;
        }

        ~shared_ptr() {
            release();
        }
    };
} // namespace sp

#endif