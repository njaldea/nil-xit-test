#pragma once

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_WRAP(A) A
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_CONCAT_D(A, B) A##B
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_CONCAT(A, B) XIT_CONCAT_D(A, B)
// clang-format off
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_IIFE(X) []() { X; return 0; }()
// clang-format on

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_TEST(SUITE, CASE, DIRECTORY)                                                           \
    struct xit_test_##SUITE##_##CASE final: XIT_WRAP(SUITE)                                        \
    {                                                                                              \
        void run(const inputs_t& xit_inputs, outputs_t& xit_outputs) override;                     \
    };                                                                                             \
    const auto v_xit_test_##SUITE##_##CASE = XIT_IIFE(                                             \
        nil::xit::gtest::get_instance()                                                            \
            .test_builder                                                                          \
            .add_test<xit_test_##SUITE##_##CASE>(#SUITE, #CASE, DIRECTORY, __FILE__, __LINE__)     \
    );                                                                                             \
    void xit_test_##SUITE##_##CASE::run(const inputs_t& xit_inputs, outputs_t& xit_outputs)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_FRAME_MAIN(PATH, CONVERTER)                                                            \
    const auto& XIT_CONCAT(xit_test_frame_, __COUNTER__)                                           \
        = XIT_IIFE(nil::xit::gtest::get_instance().main_builder.create_main(                       \
            PATH,                                                                                  \
            [](const std::vector<std::string>& v) { return CONVERTER(v); }                         \
        ))

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_FRAME_DETAIL(ID, IMPL)                                                                 \
    template <>                                                                                    \
    struct nil::xit::gtest::Frame<ID>                                                              \
    {                                                                                              \
        using type = std::remove_cvref_t<decltype(nil::xit::gtest::get_instance().IMPL)>::type;    \
        static constexpr auto* value = ID;                                                         \
    };                                                                                             \
    const auto& XIT_CONCAT(xit_test_frame_, __COUNTER__) = nil::xit::gtest::get_instance().IMPL

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_FRAME_TAGGED_INPUT(ID, PATH, INITIALIZER)                                              \
    XIT_FRAME_DETAIL(ID, frame_builder.create_tagged_input(ID, PATH, INITIALIZER))

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_FRAME_UNIQUE_INPUT(ID, PATH, INITIALIZER)                                              \
    XIT_FRAME_DETAIL(ID, frame_builder.create_unique_input(ID, PATH, INITIALIZER))

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_FRAME_OUTPUT(ID, PATH, TYPE)                                                           \
    XIT_FRAME_DETAIL(ID, frame_builder.create_output<TYPE>(ID, PATH))

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_PATH_SERVER_DIRECTORY(PATH)                                                            \
    const auto v_xit_path_server = XIT_IIFE(nil::xit::gtest::get_instance().paths.server = (PATH))

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_PATH_MAIN_UI_DIRECTORY(PATH)                                                           \
    const auto v_xit_path_main_ui = XIT_IIFE(nil::xit::gtest::get_instance().paths.main_ui = (PATH))

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_PATH_UI_DIRECTORY(PATH)                                                                \
    const auto v_xit_path_ui = XIT_IIFE(nil::xit::gtest::get_instance().paths.ui = (PATH))

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define XIT_PATH_TEST_DIRECTORY(PATH)                                                              \
    const auto v_xit_path_test = XIT_IIFE(nil::xit::gtest::get_instance().paths.test = (PATH))
