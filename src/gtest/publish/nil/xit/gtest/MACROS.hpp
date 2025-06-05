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

// --- tests
#define XIT_TEST_DEFINE(BASE, SUITE, CASE, PATH)                                                   \
    static_assert(nil::xit::gtest::is_valid_test<BASE>);                                           \
    struct xit_test_##SUITE##_##CASE final: XIT_WRAP(BASE)                                         \
    {                                                                                              \
        void run(const inputs_t& xit_inputs, outputs_t& xit_outputs) override;                     \
    };                                                                                             \
    const auto v_xit_test_##SUITE##_##CASE = XIT_IIFE(                                             \
        XIT_INSTANCE.test_builder                                                                  \
            .add_test<xit_test_##SUITE##_##CASE>(#SUITE, #CASE, XIT_FG(PATH), __FILE__, __LINE__)  \
    );                                                                                             \
    void xit_test_##SUITE##_##CASE::run(                                                           \
        [[maybe_unused]] const inputs_t& xit_inputs,                                               \
        [[maybe_unused]] outputs_t& xit_outputs                                                    \
    )

#define XIT_TEST_DEFINE_DEFAULT(BASE, SUITE, CASE) XIT_TEST_DEFINE(BASE, SUITE, CASE, ".")

#define XIT_TEST_OVERLOAD(_0, _1, NAME, ...) NAME

#define XIT_TEST(SUITE, CASE) XIT_TEST_DEFINE_DEFAULT(nil::xit::gtest::Test<>, SUITE, CASE)

#define XIT_TEST_F(SUITE, ...)                                                                     \
    XIT_TEST_OVERLOAD(__VA_ARGS__, XIT_TEST_DEFINE, XIT_TEST_DEFINE_DEFAULT)                       \
    (SUITE, SUITE, __VA_ARGS__)

#define XIT_FRAME_MAIN(PATH, ...)                                                                  \
    const auto xit_test_main_frame = XIT_IIFE(XIT_INSTANCE.main_builder.create_main(               \
        XIT_FG(PATH),                                                                              \
        [](const auto& v) { return __VA_ARGS__(v); }                                               \
    ))

#define XIT_FRAME_DETAIL(ID, SUFFIX, ...)                                                          \
    template <>                                                                                    \
    struct nil::xit::gtest::detail::Frame<ID>                                                      \
    {                                                                                              \
        using type = std::remove_cvref_t<decltype(XIT_INSTANCE.frame_builder.__VA_ARGS__)>::type;  \
        static constexpr auto* value = ID;                                                         \
        static constexpr auto* marked_value = ID SUFFIX;                                           \
        static const void* const holder;                                                           \
    };                                                                                             \
    const void* const nil::xit::gtest::detail::Frame<ID>::holder                                   \
        = &XIT_INSTANCE.frame_builder.__VA_ARGS__

#define XIT_FRAME_TAGGED_INPUT(ID, ...)                                                            \
    static_assert(nil::xit::gtest::builders::is_loader_tagged<decltype(__VA_ARGS__)>);             \
    XIT_FRAME_DETAIL(ID, ":T:N", create_tagged_input(ID, {}, XIT_WRAP_R(__VA_ARGS__)))
#define XIT_FRAME_TAGGED_INPUT_V(ID, PATH, ...)                                                    \
    static_assert(nil::xit::gtest::builders::is_loader_tagged<decltype(__VA_ARGS__)>);             \
    XIT_FRAME_DETAIL(ID, ":T:V", create_tagged_input(ID, XIT_FG(PATH), XIT_WRAP_R(__VA_ARGS__)))

#define XIT_FRAME_UNIQUE_INPUT(ID, ...)                                                            \
    static_assert(nil::xit::gtest::builders::is_loader_unique<decltype(__VA_ARGS__)>);             \
    XIT_FRAME_DETAIL(ID, ":U:N", create_unique_input(ID, {}, XIT_WRAP_R(__VA_ARGS__)))
#define XIT_FRAME_UNIQUE_INPUT_V(ID, PATH, ...)                                                    \
    static_assert(nil::xit::gtest::builders::is_loader_unique<decltype(__VA_ARGS__)>);             \
    XIT_FRAME_DETAIL(ID, ":U:V", create_unique_input(ID, XIT_FG(PATH), XIT_WRAP_R(__VA_ARGS__)))

#define XIT_FRAME_OUTPUT(ID, ...) XIT_FRAME_DETAIL(ID, ":T:N", create_output<__VA_ARGS__>(ID, {}))
#define XIT_FRAME_OUTPUT_V(ID, PATH, ...)                                                          \
    XIT_FRAME_DETAIL(ID, ":T:V", create_output<__VA_ARGS__>(ID, XIT_FG(PATH)))
