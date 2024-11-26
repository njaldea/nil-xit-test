#pragma once

#include <utility>

namespace nil::xit::gtest
{
    template <typename C, typename M>
    auto from_member(M C::*member_ptr)
    {
        struct Accessor
        {
            M get(const C& data) const
            {
                return data.*member_ptr;
            }

            void set(C& data, M new_data) const
            {
                data.*member_ptr = std::move(new_data);
            }

            M C::*member_ptr;
        };

        return Accessor{member_ptr};
    }
}
