#pragma once

#include <string_view>

namespace nil::xit::gtest
{
    template <typename T>
    auto from_data(T data)
    {
        struct Loader final
        {
            auto initialize() const
            {
                return data;
            }

            auto initialize(std::string_view /* tag */) const
            {
                return data;
            }

            T data;
        };

        return Loader{std::move(data)};
    }
}
