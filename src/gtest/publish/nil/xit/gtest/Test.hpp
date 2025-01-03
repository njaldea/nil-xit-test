#pragma once

#include "Data.hpp"

#include <algorithm>

namespace nil::xit::gtest
{
    template <std::size_t N>
    struct StringLiteral
    {
        // NOLINTNEXTLINE
        constexpr StringLiteral(const char (&str)[N])
        {
            std::copy_n(&str[0], N, &value[0]);
        }

        // NOLINTNEXTLINE
        char value[N];
    };

    template <StringLiteral S>
    struct Frame;

    template <>
    struct Frame<"index">
    {
    };

    template <>
    struct Frame<"frame_info">
    {
    };

    template <StringLiteral S>
    using frame_t = typename Frame<S>::type;
    template <StringLiteral S>
    constexpr auto frame_v = Frame<S>::value;

    template <StringLiteral... T>
    struct Input;
    template <StringLiteral... T>
    struct Output;

    template <typename... T>
    struct Test;

    template <StringLiteral... I, StringLiteral... O>
    struct Test<Input<I...>, Output<O...>>
    {
        Test() = default;
        virtual ~Test() noexcept = default;
        Test(Test&&) = delete;
        Test(const Test&) = delete;
        Test& operator=(Test&&) = delete;
        Test& operator=(const Test&) = delete;

        using base_t = Test<Input<I...>, Output<O...>>;
        using inputs_t = Data<const frame_t<I>...>;
        using outputs_t = Data<frame_t<O>...>;

        virtual void setup() {};
        virtual void teardown() {};
        virtual void run(const inputs_t& xit_inputs, outputs_t& xit_outputs) = 0;
    };

    template <>
    struct Test<>: Test<Input<>, Output<>>
    {
    };

    template <StringLiteral... S>
    using TestInputs = Test<Input<S...>, Output<>>;

    template <StringLiteral... S>
    using TestOutputs = Test<Input<>, Output<S...>>;

    template <typename T>
    concept is_valid_test = requires(T& t) {
        typename T::base_t;
        typename T::inputs_t;
        typename T::outputs_t;
        { t.setup() };
        { t.teardown() };
        {
            t.run(
                std::declval<const typename T::inputs_t&>(),
                std::declval<typename T::outputs_t&>()
            )
        };
    };
}
