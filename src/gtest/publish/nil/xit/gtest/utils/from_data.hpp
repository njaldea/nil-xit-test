#pragma once

#include <string_view>

namespace nil::xit::gtest
{
    template <typename T>
    struct from_data final
    {
        explicit from_data(T init_data)
            : data(std::move(init_data))
        {
        }

        auto initialize() const
        {
            return data;
        }

        auto initialize(std::string_view /* tag */) const
        {
            return data;
        }

        template <typename U>
        T& operator()(U& /* data */)
        {
            return data;
        }

        template <typename U>
        const T& operator()(const U& /* data */) const
        {
            return data;
        }

        T data;
    };
}
