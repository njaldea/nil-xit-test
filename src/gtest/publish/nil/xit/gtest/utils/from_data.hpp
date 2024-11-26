#pragma once

#include <string_view>

namespace nil::xit::gtest
{
    template <typename T>
    auto from_data(T data)
    {
        struct Loader final
        {
            auto load(std::string_view /* tag */) const
            {
                return data;
            }

            auto load() const
            {
                return data;
            }

            T data;
        };

        return Loader{std::move(data)};
    }
}
