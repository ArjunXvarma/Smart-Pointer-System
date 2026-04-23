#include "shared_ptr.hpp"

namespace sp {
    template<typename T, typename Alloc, typename... Args>
    sp::shared_ptr<T> allocate_shared(const Alloc& alloc, Args&&... args) {
        using CB = inplace_control_block_alloc<T, Alloc>;
        using AllocTraits = std::allocator_traits<Alloc>;
        using CBAlloc = AllocTraits::template rebind_alloc<CB>;

        CBAlloc cb_alloc(alloc);

        CB* cb = std::allocator_traits<CBAlloc>::allocate(cb_alloc, 1);

        try {
            std::allocator_traits<CBAlloc>::construct(cb_alloc, cb, alloc, std::forward<Args>(args)...);
        }
        catch(const std::exception& e) {
            std::allocator_traits<CBAlloc>::deallocate(cb_alloc, cb, 1);
            throw;
        }
        
        sp::shared_ptr<T> shared;
        shared.cb = cb;
        shared.ptr = cb->get();

        return shared;
    }
}