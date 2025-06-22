#pragma once

namespace nil::xit::gtest
{
    template <typename C, typename M>
    struct from_member
    {
        explicit from_member(M C::*init_member_ptr)
            : member_ptr(init_member_ptr)
        {
        }

        M& operator()(C& data)
        {
            return data.*member_ptr;
        }

        const M& operator()(const C& data) const
        {
            return data.*member_ptr;
        }

        M C::*member_ptr;
    };
}
