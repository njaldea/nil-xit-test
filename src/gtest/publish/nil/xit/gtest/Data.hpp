#pragma once

#include <tuple>
#include <type_traits>

namespace nil::xit::gtest
{
    template <typename... T>
    struct Data
    {
        std::tuple<T* const...> data;
    };

    template <std::size_t I, typename... T>
        requires(I < sizeof...(T))
    auto& get(const Data<T...>& o)
    {
        return *std::get<I>(o.data);
    }
}

template <typename... T>
struct std::tuple_size<nil::xit::gtest::Data<T...>>
    : std::integral_constant<std::size_t, sizeof...(T)>
{
};

template <std::size_t I, typename... T>
    requires(I < sizeof...(T))
struct std::tuple_element<I, nil::xit::gtest::Data<T...>>
{
    using type = std::remove_cvref_t< //
        decltype(*std::get<I>(std::declval<nil::xit::gtest::Data<T...>>().data))>;
};
