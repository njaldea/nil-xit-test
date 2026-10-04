// Copyright (c) 2026, Neil Aldea <njaldea@gmail.com>
// SPDX-License-Identifier: BSL-1.0
// See the repository LICENSE and https://www.boost.org/LICENSE_1_0.txt.

#pragma once

#include <tuple>
#include <type_traits>

namespace nil::xit::gtest
{
    template <typename... T>
    struct Data final
    {
        std::tuple<T* const...> data;
    };

    template <std::size_t I, typename... T>
        requires(I < sizeof...(T))
    auto& get(const Data<T...>& o)
    {
        return *get<I>(o.data);
    }
}

template <typename... T>
struct std::tuple_size<nil::xit::gtest::Data<T...>>
    : std::integral_constant<std::size_t, sizeof...(T)>
{
};

template <std::size_t I, typename... T>
struct std::tuple_element<I, nil::xit::gtest::Data<T...>>
{
    using type = std::remove_cvref_t<std::tuple_element_t<I, std::tuple<T...>>>;
};
