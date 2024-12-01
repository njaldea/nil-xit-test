#pragma once

#include <concepts> // IWYU pragma: keep

namespace nil::xit::gtest
{
    template <typename C, typename M>
    auto from_member(M C::*member_ptr)
    {
        struct Accessor
        {
            // clang-format off
            M& operator()(C& data) const { return data.*member_ptr; }
            const M& operator()(const C& data) const { return data.*member_ptr; }
            M C::*member_ptr;
            // clang-format on
        };

        return Accessor{member_ptr};
    }
}
