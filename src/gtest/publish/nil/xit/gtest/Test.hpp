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
        using inputs_t = Data<const typename Frame<I>::type...>;
        using outputs_t = Data<typename Frame<O>::type...>;

        virtual void setup() {};
        virtual void teardown() {};
        virtual void run(const inputs_t& xit_inputs, outputs_t& xit_outputs) = 0;
    };

    template <>
    struct Test<>: Test<Input<>, Output<>>
    {
    };
}
