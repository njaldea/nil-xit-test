#pragma once

#include <type_traits>

namespace nil::xit::gtest
{
    template <typename... T>
        requires(std::is_same_v<T, std::remove_cvref_t<T>> && ...)
    struct type
    {
    };
}
