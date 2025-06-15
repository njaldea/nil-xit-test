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
    static_assert(nil::xit::gtest::is_valid_test<BASE>);                                           \
    static_assert(nil::xit::gtest::is_valid_path<PATH>);                                           \
    static_assert(BASE::are_inputs_valid);                                                         \
    static_assert(BASE::are_outputs_valid);                                                        \
    struct xit_test_##SUITE##_##CASE final: XIT_WRAP(BASE)                                         \
    {                                                                                              \
        void run(const inputs_t& xit_inputs, outputs_t& xit_outputs) override;                     \
    };                                                                                             \
    const auto v_xit_test_##SUITE##_##CASE = XIT_IIFE(                                             \
        XIT_INSTANCE.paths.expected_groups.emplace(XIT_FG(PATH).group);                            \
        XIT_INSTANCE.test_builder                                                                  \
            .add_test<xit_test_##SUITE##_##CASE>(#SUITE, #CASE, XIT_FG(PATH), __FILE__, __LINE__)  \
    );                                                                                             \
    void xit_test_##SUITE##_##CASE::run(                                                           \
        [[maybe_unused]] const inputs_t& xit_inputs,                                               \
        [[maybe_unused]] outputs_t& xit_outputs                                                    \
    )

#define XIT_TEST_F(SUITE, CASE, PATH) XIT_TEST_DETAIL(SUITE, SUITE, CASE, PATH)
#define XIT_TEST(SUITE, CASE, PATH) XIT_TEST_DETAIL(nil::xit::gtest::Test<>, SUITE, CASE, PATH)

#define XIT_FRAME_MAIN(PATH, ...)                                                                  \
    inline const auto xit_test_main_frame = XIT_IIFE(                                              \
        XIT_INSTANCE.main_builder.create_main(PATH, [](const auto& v) { return __VA_ARGS__(v); }); \
        if constexpr (nil::xalt::starts_with<PATH, "$">()) {                                       \
            XIT_INSTANCE.paths.expected_groups.emplace(nil::xit::gtest::detail::get_fg_name<PATH>( \
            ));                                                                                    \
        }                                                                                          \
    )

#define XIT_FRAME_DETAIL(ID, SUFFIX, TYPE, ...)                                                    \
    template <>                                                                                    \
    struct nil::xit::gtest::detail::Frame<ID>                                                      \
    {                                                                                              \
        using r_type = decltype(XIT_INSTANCE.frame_builder.create_##__VA_ARGS__);                  \
        using type = std::remove_cvref_t<r_type>::type;                                            \
        static constexpr auto* value = ID;                                                         \
        static constexpr auto* marked_value = ID SUFFIX;                                           \
        static const void* const holder;                                                           \
        static constexpr auto frame_type = nil::xit::gtest::detail::EFrameType::TYPE;              \
    };                                                                                             \
    inline const void* const nil::xit::gtest::detail::Frame<ID>::holder                            \
        = &XIT_INSTANCE.frame_builder.create_##__VA_ARGS__

// clang-format off
#define XIT_FRAME_TEST_INPUT(ID, ...)                                                                           \
    static_assert(!nil::xit::gtest::builders::is_loader_unique<decltype(__VA_ARGS__)>);                         \
    XIT_FRAME_DETAIL(ID, ":T:N", Input, test_input(ID, {}, XIT_WRAP_R(__VA_ARGS__)))
#define XIT_FRAME_TEST_INPUT_V(ID, PATH, ...)                                                                   \
    static_assert(!nil::xit::gtest::builders::is_loader_unique<decltype(__VA_ARGS__)>);                         \
    XIT_FRAME_DETAIL(ID, ":T:V", Input, test_input(ID, PATH, XIT_WRAP_R(__VA_ARGS__)))
#define XIT_FRAME_TEST_V(ID, PATH)                                                                              \
    XIT_FRAME_DETAIL(ID, ":T:V", Utility, test_input(ID, PATH, XIT_WRAP_R(nil::xit::gtest::none())))

#define XIT_FRAME_GLOBAL_INPUT(ID, ...)                                                                         \
    static_assert(!nil::xit::gtest::builders::is_test_loader<decltype(__VA_ARGS__)>);                           \
    XIT_FRAME_DETAIL(ID, ":U:N", Input, global_input(ID, {}, XIT_WRAP_R(__VA_ARGS__)))
#define XIT_FRAME_GLOBAL_INPUT_V(ID, PATH, ...)                                                                 \
    static_assert(!nil::xit::gtest::builders::is_test_loader<decltype(__VA_ARGS__)>);                           \
    XIT_FRAME_DETAIL(ID, ":U:V", Input, global_input(ID, PATH, XIT_WRAP_R(__VA_ARGS__)))
#define XIT_FRAME_GLOBAL_V(ID, PATH)                                                                            \
    XIT_FRAME_DETAIL(ID, ":U:V", Utility, global_input(ID, PATH, XIT_WRAP_R(nil::xit::gtest::none())))

#define XIT_FRAME_OUTPUT(ID, ...)                                                                               \
    XIT_FRAME_DETAIL(ID, ":T:N", output<__VA_ARGS__>(ID, {}))
#define XIT_FRAME_OUTPUT_V(ID, PATH, ...)                                                                       \
    XIT_FRAME_DETAIL(ID, ":T:V", Output, output<__VA_ARGS__>(ID, PATH))
// clang-format on
