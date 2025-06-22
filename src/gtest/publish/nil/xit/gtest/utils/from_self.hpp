#pragma once

namespace nil::xit::gtest
{
    template <typename C>
    struct from_self
    {
        C& operator()(C& data)
        {
            return data;
        }

        const C& operator()(const C& data) const
        {
            return data;
        }
    };
}
