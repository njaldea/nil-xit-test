#pragma once

#include "Instances.hpp" // IWYU pragma: keep
#include "Test.hpp"      // IWYU pragma: keep

#define XIT_WRAP(A) A
// clang-format off
#define XIT_IIFE(X) []() { X; return 0; }()
// clang-format on

#define XIT_INSTANCE nil::xit::gtest::get_instance()

#define XIT_TEST_DEFINE(BASE, SUITE, CASE, DIRECTORY)                                              \
    static_assert(nil::xit::gtest::is_valid_test<BASE>);                                           \
    struct xit_test_##SUITE##_##CASE final: XIT_WRAP(BASE)                                         \
    {                                                                                              \
        void run(const inputs_t& xit_inputs, outputs_t& xit_outputs) override;                     \
    };                                                                                             \
    const auto v_xit_test_##SUITE##_##CASE = XIT_IIFE(                                             \
        XIT_INSTANCE.test_builder                                                                  \
            .add_test<xit_test_##SUITE##_##CASE>(#SUITE, #CASE, DIRECTORY, __FILE__, __LINE__)     \
    );                                                                                             \
    void xit_test_##SUITE##_##CASE::run(                                                           \
        [[maybe_unused]] const inputs_t& xit_inputs,                                               \
        [[maybe_unused]] outputs_t& xit_outputs                                                    \
    )

#define XIT_TEST_DEFINE_DEFAULT(BASE, SUITE, CASE) XIT_TEST_DEFINE(BASE, SUITE, CASE, ".")

#define XIT_TEST_OVERLOAD(_0, _1, NAME, ...) NAME

#define XIT_TEST(SUITE, CASE)                                                                      \
    XIT_TEST_DEFINE_DEFAULT(                                                                       \
        nil::xit::gtest::Test<nil::xit::gtest::Input<>, nil::xit::gtest::Output<>>,                \
        SUITE,                                                                                     \
        CASE                                                                                       \
    )

#define XIT_TEST_F(SUITE, ...)                                                                     \
    XIT_TEST_OVERLOAD(__VA_ARGS__, XIT_TEST_DEFINE, XIT_TEST_DEFINE_DEFAULT)                       \
    (SUITE, SUITE, __VA_ARGS__)

#define XIT_FRAME_MAIN(PATH, CONVERTER)                                                            \
    const auto xit_test_main_frame = XIT_IIFE(                                                     \
        XIT_INSTANCE.main_builder.create_main(PATH, [](const auto& v) { return CONVERTER(v); })    \
    )

#define XIT_FRAME_DETAIL(ID, SUFFIX, IMPL)                                                         \
    template <>                                                                                    \
    struct nil::xit::gtest::detail::Frame<ID>                                                      \
    {                                                                                              \
        using type = std::remove_cvref_t<decltype(XIT_INSTANCE.frame_builder.IMPL)>::type;         \
        static constexpr auto* value = ID;                                                         \
        static constexpr auto* marked_value = ID SUFFIX;                                           \
        static const void* const holder;                                                           \
    };                                                                                             \
    const void* const nil::xit::gtest::detail::Frame<ID>::holder = &XIT_INSTANCE.frame_builder.IMPL

#define XIT_FRAME_TAGGED_INPUT(ID, PATH, INITIALIZER)                                              \
    XIT_FRAME_DETAIL(ID, ":T", create_tagged_input(ID, PATH, []() { return INITIALIZER; }))

#define XIT_FRAME_UNIQUE_INPUT(ID, PATH, INITIALIZER)                                              \
    XIT_FRAME_DETAIL(ID, ":U", create_unique_input(ID, PATH, []() { return INITIALIZER; }))

#define XIT_FRAME_OUTPUT(ID, PATH, TYPE) XIT_FRAME_DETAIL(ID, ":T", create_output<TYPE>(ID, PATH))

#define XIT_PATH_SERVER_DIRECTORY(PATH)                                                            \
    const auto v_xit_path_server = XIT_IIFE(XIT_INSTANCE.paths.server = (PATH))

#define XIT_PATH_MAIN_UI_DIRECTORY(PATH)                                                           \
    const auto v_xit_path_main_ui = XIT_IIFE(XIT_INSTANCE.paths.main_ui = (PATH))

#define XIT_PATH_UI_DIRECTORY(PATH)                                                                \
    const auto v_xit_path_ui = XIT_IIFE(XIT_INSTANCE.paths.ui = (PATH))

#define XIT_PATH_TEST_DIRECTORY(PATH)                                                              \
    const auto v_xit_path_test = XIT_IIFE(XIT_INSTANCE.paths.test = (PATH))
