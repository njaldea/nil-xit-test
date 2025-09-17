#pragma once

#include "Instances.hpp" // IWYU pragma: keep
#include "Test.hpp"      // IWYU pragma: keep

#define XIT_WRAP(A) A
// clang-format off
#define XIT_IIFE(X) []() { X; return 0; }()
#define XIT_WRAP_R(...) []() { return __VA_ARGS__; }
#define XIT_FG(P) nil::xit::gtest::detail::get_file_info<P>()
// clang-format on

#define XIT_INSTANCE nil::xit::gtest::get_instance()

#define XIT_TEST_DETAIL(BASE, SUITE, CASE, PATH)                                                   \
    static_assert(nil::xit::gtest::is_valid_path<PATH>);                                           \
    struct xit_test_##SUITE##_##CASE: XIT_WRAP(BASE)                                               \
    {                                                                                              \
    };                                                                                             \
    const auto v_xit_test_##SUITE##_##CASE = XIT_IIFE(                                             \
        XIT_INSTANCE.paths.used_groups.emplace(XIT_FG(PATH).group);                                \
        XIT_INSTANCE.test_builder                                                                  \
            .add_test<xit_test_##SUITE##_##CASE>(#SUITE, #CASE, XIT_FG(PATH), __FILE__, __LINE__)  \
    );                                                                                             \
    template <>                                                                                    \
    void nil::xit::gtest::Test<xit_test_##SUITE##_##CASE>::run(                                    \
        [[maybe_unused]] const input_frames::type& xit_inputs,                                     \
        [[maybe_unused]] output_frames::type& xit_outputs,                                         \
        [[maybe_unused]] expect_frames::type& xit_expects                                          \
    )

#define XIT_INPUTS(...) using input_frames = nil::xit::gtest::Inputs<__VA_ARGS__>
#define XIT_OUTPUTS(...) using output_frames = nil::xit::gtest::Outputs<__VA_ARGS__>
#define XIT_EXPECTS(...) using expect_frames = nil::xit::gtest::Expects<__VA_ARGS__>

// clang-format off
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
#define XIT_TEST(SUITE, CASE, PATH) XIT_TEST_DETAIL(nil::xit::gtest::detail::Default, SUITE, CASE, PATH)

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
#define XIT_TEST_F(SUITE, CASE, PATH) XIT_TEST_DETAIL(SUITE, SUITE, CASE, PATH)
// clang-format on

#define XIT_FRAME_MAIN(PATH, ...)                                                                  \
    inline const auto xit_test_main_frame = XIT_IIFE(                                              \
        XIT_INSTANCE.main_builder.create_main(PATH);                                               \
        if constexpr (nil::xalt::starts_with<PATH, "$">()) {                                       \
            XIT_INSTANCE.paths.used_groups.emplace(nil::xit::gtest::detail::get_fg_name<PATH>());  \
        }                                                                                          \
    )

#define XIT_FRAME_DETAIL(ID, SUFFIX, ETYPE, ...)                                                   \
    template <>                                                                                    \
    struct nil::xit::gtest::detail::Frame<ID>                                                      \
    {                                                                                              \
        using type = std::remove_cvref_t<decltype(XIT_INSTANCE.frame_builder.__VA_ARGS__)>::type;  \
        static constexpr auto* value = ID;                                                         \
        static constexpr auto* marked_value = ID SUFFIX;                                           \
        static constexpr auto frame_type = nil::xit::gtest::detail::EFrameType::ETYPE;             \
        static const void* const holder;                                                           \
    };                                                                                             \
    inline const void* const nil::xit::gtest::detail::Frame<ID>::holder                            \
        = &XIT_INSTANCE.frame_builder.__VA_ARGS__

// clang-format off
#define XIT_FRAME_TEST_INPUT(ID, ...)                                                                           \
    static_assert(!nil::xit::gtest::builders::is_loader_unique<decltype(__VA_ARGS__)>);                         \
    XIT_FRAME_DETAIL(ID, ":T:N", Input, create_test_input(ID, {}, XIT_WRAP_R(__VA_ARGS__)))
#define XIT_FRAME_TEST_INPUT_V(ID, PATH, ...)                                                                   \
    static_assert(!nil::xit::gtest::builders::is_loader_unique<decltype(__VA_ARGS__)>);                         \
    XIT_FRAME_DETAIL(ID, ":T:V", Input, create_test_input(ID, PATH, XIT_WRAP_R(__VA_ARGS__)))
#define XIT_FRAME_TEST_V(ID, PATH)                                                                              \
    XIT_FRAME_DETAIL(ID, ":T:V", Utility, create_test_input(ID, PATH, XIT_WRAP_R(nil::xit::gtest::none())))

#define XIT_FRAME_GLOBAL_INPUT(ID, ...)                                                                         \
    static_assert(!nil::xit::gtest::builders::is_loader_tagged<decltype(__VA_ARGS__)>);                         \
    XIT_FRAME_DETAIL(ID, ":U:N", Input, create_global_input(ID, {}, XIT_WRAP_R(__VA_ARGS__)))
#define XIT_FRAME_GLOBAL_INPUT_V(ID, PATH, ...)                                                                 \
    static_assert(!nil::xit::gtest::builders::is_loader_tagged<decltype(__VA_ARGS__)>);                         \
    XIT_FRAME_DETAIL(ID, ":U:V", Input, create_global_input(ID, PATH, XIT_WRAP_R(__VA_ARGS__)))
#define XIT_FRAME_GLOBAL_V(ID, PATH)                                                                            \
    XIT_FRAME_DETAIL(ID, ":U:V", Utility, create_global_input(ID, PATH, XIT_WRAP_R(nil::xit::gtest::none())))

#define XIT_FRAME_EXPECT(ID, ...)                                                                               \
    static_assert(!nil::xit::gtest::builders::is_loader_unique<decltype(__VA_ARGS__)>);                         \
    XIT_FRAME_DETAIL(ID, ":T:N", Expect, create_expect(ID, {}, XIT_WRAP_R(__VA_ARGS__)))
#define XIT_FRAME_EXPECT_V(ID, PATH, ...)                                                                       \
    static_assert(!nil::xit::gtest::builders::is_loader_unique<decltype(__VA_ARGS__)>);                         \
    XIT_FRAME_DETAIL(ID, ":T:V", Expect, create_expect(ID, PATH, XIT_WRAP_R(__VA_ARGS__)))

#define XIT_FRAME_OUTPUT(ID, ...)                                                                               \
    XIT_FRAME_DETAIL(ID, ":T:N", Output, create_output<__VA_ARGS__>(ID, {}))
#define XIT_FRAME_OUTPUT_V(ID, PATH, ...)                                                                       \
    XIT_FRAME_DETAIL(ID, ":T:V", Output, create_output<__VA_ARGS__>(ID, PATH))
// clang-format on

// Assumption is that GTEST only calls this on failure.
#undef GTEST_MESSAGE_AT_
#define GTEST_MESSAGE_AT_(file, line, message, result_type)                                        \
    ::testing::internal::AssertHelper(result_type, file, line, message) = []()                     \
    {                                                                                              \
        XIT_INSTANCE.tracker.set_result(false);                                                    \
        return ::testing::Message();                                                               \
    }()
