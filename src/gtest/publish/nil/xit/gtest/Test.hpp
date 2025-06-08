#pragma once

#include "Data.hpp"

#include <nil/xalt/literal.hpp>
#include <nil/xalt/str_name.hpp>

#include <nil/xit/structs.hpp>

namespace nil::xit::gtest
{
    namespace detail
    {
        template <xalt::literal lit>
        consteval auto get_fg_name()
        {
            static_assert(xalt::starts_with<lit, "$">());
            const auto i = xalt::find_first<lit, "/">();
            return xalt::literal_v<xalt::substr<lit, 1, i - 1>()>; // NOLINT
        }

        template <xalt::literal lit>
        auto get_fg_path()
        {
            static_assert(xalt::starts_with<lit, "$">());
            const auto i = xalt::find_first<lit, "/">();
            return xalt::literal_v<xalt::substr<lit, i + 1, sizeof(lit) - i - 2>()>; // NOLINT
        }

        template <xalt::literal lit>
        auto get_file_info()
        {
            return FileInfo{
                get_fg_name<lit>(),
                get_fg_path<lit>(),
            };
        }

        template <xalt::literal S>
        struct Frame;

        // using type = std::remove_cvref_t<decltype(XIT_INSTANCE.frame_builder.__VA_ARGS__)>::type;
        // static constexpr auto* marked_value = ID SUFFIX;
        // static constexpr auto* value = ID;
        // static const void* const holder;

        enum class EFrameType
        {
            Utility,
            Input,
            Output
        };

        template <>
        struct Frame<"index"> final
        {
            // this is reserved for main gui
            static constexpr auto frame_type = EFrameType::Utility;
        };

        template <>
        struct Frame<"frame_info"> final
        {
            // this is reserved for getting frame details
            // of a specific tag (test)
            static constexpr auto frame_type = EFrameType::Utility;
        };
    }

    template <xalt::literal... T>
        requires(true && ... && (detail::Frame<T>::frame_type == detail::EFrameType::Input))
    struct Input
    {
    };

    template <xalt::literal... T>
        requires(true && ... && (detail::Frame<T>::frame_type == detail::EFrameType::Output))
    struct Output
    {
    };

    template <typename I = Input<>, typename O = Output<>>
    struct Test;

    template <xalt::literal... I, xalt::literal... O>
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

        static constexpr auto are_inputs_valid
            = (true && ... && (detail::Frame<I>::frame_type == detail::EFrameType::Input));

        static constexpr auto are_outputs_valid
            = (true && ... && (detail::Frame<O>::frame_type == detail::EFrameType::Output));
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

    template <xalt::literal lit>
    concept is_valid_path               //
        = xalt::starts_with<lit, "$">() //
        && (xalt::find_first<lit, "/">() < sizeof(lit));

    struct none
    {
        bool operator==(const none& /* o */) const
        {
            return false;
        }
    };
}
