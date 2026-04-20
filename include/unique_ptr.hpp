//
// Created by Arjun Varma on 18/04/26.
//

#ifndef UNIQUE_PTR_HPP
#define UNIQUE_PTR_HPP

#include<memory>

namespace sp {
    template<typename T, typename Deleter = std::default_delete<T>>
    class unique_ptr : private Deleter {
    private:
        T* ptr;

    public:
        unique_ptr() noexcept : ptr(nullptr) {}
        explicit unique_ptr(T* ptr) noexcept : ptr(ptr) {}
        unique_ptr(T* ptr, Deleter d) noexcept : Deleter(std::move(d)), ptr(ptr) {}

        // Delete copy semantics
        unique_ptr(const unique_ptr&) = delete;
        unique_ptr& operator=(const unique_ptr&) = delete;

        // Move semantics
        // Move constructor
        unique_ptr(unique_ptr&& other) noexcept 
        : Deleter(std::move(other.get_deleter())), ptr(other.ptr) {
            other.ptr = nullptr;
        }

        // Move assignment operator
        unique_ptr& operator=(unique_ptr&& other) noexcept {
            if (other != this) {
                reset();
                ptr = other.ptr;
                other.ptr = nullptr;

                get_deleter() = std::move(other.get_deleter());
            }

            return *this;
        }

        void reset(T* p = nullptr) noexcept {
            if (ptr) get_deleter()(ptr);

            ptr = p;
        }

        T* release() noexcept {
            T* temp = ptr;
            ptr = nullptr;
            return temp;
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

        Deleter& get_deleter() noexcept {
            return static_cast<Deleter&>(*this);
        }

        const Deleter& get_deleter() const noexcept {
            return static_cast<const Deleter&>(*this);
        }

        ~unique_ptr() {
            if (ptr) {
                get_deleter()(ptr);
            }
        }
    };
}

#endif //UNIQUE_PTR_HPP
