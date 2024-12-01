#pragma once

#include <utility>

namespace nil::xit::test::utils
{
    template <typename T>
    struct NoImplicit
    {
        operator T() const; // NOLINT
    };

    template <typename Callable, typename... T>
    using return_t = decltype(std::declval<Callable>()(utils::NoImplicit<T>()...));
}
