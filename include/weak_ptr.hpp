//
// Created by Arjun Varma on 22/04/26.
//

#ifndef WEAK_PTR_HPP
#define WEAK_PTR_HPP

#include "control_block.hpp"
#include "shared_ptr.hpp"

namespace sp {
    template<typename T>
    class weak_ptr {
    private:
        control_block_base* cb = nullptr;
        T* ptr = nullptr;
    public:
        weak_ptr() noexcept = default;

        weak_ptr(const sp::shared_ptr<T>& p) noexcept {
            cb = p.cb;
            ptr = p.ptr;

            if (cb) cb->add_weak();
        }

        weak_ptr(const weak_ptr& other) noexcept {
            cb = other.cb;
            ptr = other.ptr;

            if (cb) cb->add_weak();
        }

        weak_ptr(weak_ptr&& other) noexcept{
            cb = other.cb;
            ptr = other.ptr;

            other.cb = nullptr;
            other.ptr = nullptr;
        }

        weak_ptr& operator=(const weak_ptr& other) noexcept {
            if (this != &other) {
                release();
                cb = other.cb;
                ptr = other.ptr;

                if (cb) cb->add_weak();
            }

            return *this;
        }

        weak_ptr& operator=(weak_ptr&& other) noexcept {
            if (this != &other) {
                release();
                cb = other.cb;
                ptr = other.ptr;

                other.cb = nullptr;
                other.ptr = nullptr;
            }

            return *this;
        }

        void release() {
            if (!cb) return;

            cb->release_weak();

            cb = nullptr;
            ptr = nullptr;
        }

        size_t use_count() const noexcept {
            return cb ? cb->strong_count.load(std::memory_order_acquire) : 0;
        }

        bool expired() const noexcept {
            return use_count() == 0;
        }

        shared_ptr<T> lock() const noexcept {
            if (!cb) return shared_ptr<T>();

            size_t count = cb->strong_count.load(std::memory_order_acquire);

            // While the resource pointed by this exists, return shared_ptr with ownership of that resource
            while (count != 0) {
                // Only increment if object is still alive
                if (cb->strong_count.compare_exchange_weak(
                    count,
                    count + 1,
                    std::memory_order_acq_rel,
                    std::memory_order_relaxed
                )) {
                    shared_ptr<T> shared;
                    shared.ptr = ptr;
                    shared.cb = cb;

                    return shared;
                }
            }

            return shared_ptr<T>();
        }

        ~weak_ptr() {
            release();
        }
    };
} // namespace sp


#endif //WEAK_PTR_HPP