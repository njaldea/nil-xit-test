#pragma once

#include <utility>

namespace nil::xit::test::utils
{
    template <typename Callable, typename... T>
    using return_t = decltype(std::declval<Callable>()(std::declval<T>()...));
}
