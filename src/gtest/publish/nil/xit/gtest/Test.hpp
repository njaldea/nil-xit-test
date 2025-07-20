#pragma once

#include "Data.hpp"

#include <nil/xit/structs.hpp>

#include <nil/xalt/literal.hpp>

namespace nil::xit::gtest
{
    namespace detail
    {
        template <xalt::literal lit>
        consteval auto get_fg_name()
        {
            const auto i = xalt::find<lit, "/">();
            return xalt::literal_v<xalt::substr<lit, 1, i - 1>()>; // NOLINT
        }

        template <xalt::literal lit>
        auto get_fg_path()
        {
            const auto i = xalt::find<lit, "/">();
            return xalt::literal_v<xalt::substr<lit, i + 1, sizeof(lit) - i - 2>()>; // NOLINT
        }

        template <xalt::literal lit>
        auto get_file_info()
        {
            static_assert(xalt::starts_with<lit, "$">());
            static_assert(xalt::find<lit, "/">() != sizeof(lit));
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
            Output,
            Expect
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
    struct Inputs
    {
        using type = Data<const typename detail::Frame<T>::type...>;
    };

    template <xalt::literal... T>
        requires(true && ... && (detail::Frame<T>::frame_type == detail::EFrameType::Output))
    struct Outputs
    {
        using type = Data<typename detail::Frame<T>::type...>;
    };

    template <xalt::literal... T>
        requires(true && ... && (detail::Frame<T>::frame_type == detail::EFrameType::Expect))
    struct Expects
    {
        using type = Data<typename detail::Frame<T>::type...>;
    };

    namespace detail
    {
        template <typename T>
        struct InputFramesDefaulter
        {
            using type = Inputs<>;
        };

        template <typename T>
            requires requires() { typename T::input_frames; }
        struct InputFramesDefaulter<T>
        {
            using type = T::input_frames;
        };

        template <typename T>
        struct OutputFramesDefaulter
        {
            using type = Outputs<>;
        };

        template <typename T>
            requires requires() { typename T::output_frames; }
        struct OutputFramesDefaulter<T>
        {
            using type = T::output_frames;
        };

        template <typename T>
        struct ExpectFramesDefaulter
        {
            using type = Expects<>;
        };

        template <typename T>
            requires requires() { typename T::expect_frames; }
        struct ExpectFramesDefaulter<T>
        {
            using type = T::expect_frames;
        };

        struct Default
        {
        };
    }

    template <typename Fixture = detail::Default>
    struct Test final: Fixture
    {
        Test() = default;
        virtual ~Test() noexcept = default;
        Test(Test&&) = delete;
        Test(const Test&) = delete;
        Test& operator=(Test&&) = delete;
        Test& operator=(const Test&) = delete;

        using input_frames = detail::InputFramesDefaulter<Fixture>::type;
        using output_frames = detail::OutputFramesDefaulter<Fixture>::type;
        using expect_frames = detail::ExpectFramesDefaulter<Fixture>::type;

        void setup()
        {
            if constexpr (requires() { Fixture::setup(); })
            {
                Fixture::setup();
            }
        }

        void teardown()
        {
            if constexpr (requires() { Fixture::teardown(); })
            {
                Fixture::teardown();
            }
        }

        void run( //
            const input_frames::type& xit_inputs,
            output_frames::type& xit_outputs,
            expect_frames::type& xit_expects
        );
    };

    template <xalt::literal lit>
    concept is_valid_path = xalt::starts_with<lit, "$">() && (xalt::find<lit, "/">() < sizeof(lit));

    struct none
    {
    };
}
