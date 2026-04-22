#include "shared_ptr.hpp"

namespace sp {
    template<typename T, typename... Args>
    shared_ptr<T> make_shared(Args&&... args) {
        auto* cb = new inplace_control_block<T>(std::forward<Args>(args)...);

        shared_ptr<T> sp;
        sp.cb = cb;
        sp.ptr = cb->get();

        return sp;
    }
} // namespace sp
