#pragma once

#include "Data.hpp"

#include <nil/xalt/literal.hpp>

namespace nil::xit::gtest
{
    namespace detail
    {
        template <nil::xalt::literal S>
        struct Frame;

        // using type = std::remove_cvref_t<decltype(XIT_INSTANCE.frame_builder.__VA_ARGS__)>::type;
        // static constexpr auto* value = ID;
        // static constexpr auto* marked_value = ID SUFFIX;
        // static const void* const holder;

        template <>
        struct Frame<"index"> final
        {
            // this is reserved for main gui
        };

        template <>
        struct Frame<"frame_info"> final
        {
            // this is reserved for getting frame details
            // of a specific tag (test)
        };
    }

    template <nil::xalt::literal... T>
    struct Input;

    template <nil::xalt::literal... T>
    struct Output;

    template <typename... T>
    struct Test;

    template <nil::xalt::literal... I, nil::xalt::literal... O>
    struct Test<Input<I...>, Output<O...>>
    {
        Test() = default;
        virtual ~Test() noexcept = default;
        Test(Test&&) = delete;
        Test(const Test&) = delete;
        Test& operator=(Test&&) = delete;
        Test& operator=(const Test&) = delete;

        using base_t = Test<Input<I...>, Output<O...>>;
        using inputs_t = Data<const typename detail::Frame<I>::type...>;
        using outputs_t = Data<typename detail::Frame<O>::type...>;

        virtual void setup() {};
        virtual void teardown() {};
        virtual void run(const inputs_t& xit_inputs, outputs_t& xit_outputs) = 0;
    };

    template <xalt::literal... S>
    using TestInputs = Test<Input<S...>, Output<>>;

    template <xalt::literal... S>
    using TestOutputs = Test<Input<>, Output<S...>>;

    template <typename T>
    concept is_valid_test = requires(T& t) {
        typename T::base_t;
        typename T::inputs_t;
        typename T::outputs_t;
        t.setup();
        t.teardown();
        t.run(std::declval<const typename T::inputs_t&>(), std::declval<typename T::outputs_t&>());
    };
}
