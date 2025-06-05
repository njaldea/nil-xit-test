#pragma once

#include "../Test.hpp"
#include "../headless/Inputs.hpp"

#include <nil/xalt/tlist.hpp>
#include <nil/xit/structs.hpp>
#include <nil/xit/test/App.hpp>

#include <gtest/gtest.h>

#include <string>
#include <utility>

namespace nil::xit::gtest::builders
{
    std::string to_tag_suffix(std::string_view test_id, const FileInfo& file_info);

    std::string to_tag(
        std::string_view suite_id,
        std::string_view test_id,
        const FileInfo& file_info
    );

    template <typename P, nil::xalt::literal... I, nil::xalt::literal... O>
    void install(
        test::App& app,
        nil::xalt::tlist<Test<Input<I...>, Output<O...>>> /* type */,
        std::string_view suite_id,
        std::string_view test_id,
        const FileInfo& file_info
    )
    {
        using base_t = Test<Input<I...>, Output<O...>>;
        using inputs_t = typename base_t::inputs_t;
        using outputs_t = typename base_t::outputs_t;

        const auto tag = to_tag(suite_id, test_id, file_info);
        app.add_info(tag, {detail::Frame<I>::marked_value...}, {detail::Frame<O>::marked_value...});

        constexpr auto node = [](const detail::Frame<I>::type&... args)
        {
            std::tuple<typename detail::Frame<O>::type...> result;
            auto inputs = inputs_t{{&args...}};
            auto outputs = std::apply([](auto&... o) { return outputs_t{{&o...}}; }, result);
            try
            {
                P p;
                p.setup();
                p.run(inputs, outputs);
                p.teardown();
            }
            catch (const std::exception&)
            {
            }
            catch (...)
            {
            }
            return result;
        };

        app.add_node(
            tag,
            node,
            std::make_tuple( //
                app.get_input<typename detail::Frame<I>::type>(detail::Frame<I>::value)...
            ),
            std::make_tuple( //
                app.get_output<typename detail::Frame<O>::type>(detail::Frame<O>::value)...
            )
        );
    }

    template <typename P, nil::xalt::literal... I, nil::xalt::literal... O>
    void install(
        headless::Inputs& inputs,
        nil::xalt::tlist<Test<Input<I...>, Output<O...>>> /* type */,
        const std::string& suite_id,
        const std::string& test_id,
        const FileInfo& file_info,
        const char* file,
        int line
    )
    {
        using base_t = Test<Input<I...>, Output<O...>>;

        class XitTest: public ::testing::Test
        {
        public:
            XitTest(std::string init_tag, headless::Inputs* init_inputs)
                : tag(std::move(init_tag))
                , inputs(init_inputs)
            {
            }

            ~XitTest() override = default;
            XitTest(const XitTest&) = delete;
            XitTest& operator=(const XitTest&) = delete;
            XitTest(XitTest&&) noexcept = delete;
            XitTest& operator=(XitTest&&) noexcept = delete;

        private:
            void TestBody() override
            {
                using inputs_t = typename base_t::inputs_t;
                using outputs_t = typename base_t::outputs_t;

                auto input_d = std::make_tuple(
                    inputs->get<typename detail::Frame<I>::type>(detail::Frame<I>::value, tag)...
                );
                auto output_d = std::make_tuple(typename detail::Frame<O>::type()...);
                auto i = std::apply([](const auto&... ii) { return inputs_t{{&ii...}}; }, input_d);
                auto o = std::apply([](auto&... oo) { return outputs_t{{&oo...}}; }, output_d);
                P p;
                p.setup();
                p.run(i, o);
                p.teardown();
            }

            std::string tag;
            headless::Inputs* inputs;
        };

        struct Factory: ::testing::internal::TestFactoryBase
        {
            Factory(std::string init_tag, headless::Inputs* init_inputs)
                : tag(std::move(init_tag))
                , inputs(init_inputs)
            {
            }

            ::testing::Test* CreateTest() override
            {
                // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
                return new XitTest(tag, inputs);
            }

            std::string tag;
            headless::Inputs* inputs;
        };

        testing::internal::MakeAndRegisterTestInfo(
            suite_id,
            to_tag_suffix(test_id, file_info).c_str(),
            nullptr,
            nullptr,
            ::testing::internal::CodeLocation(file, line),
            ::testing::internal::GetTestTypeId(),
            ::testing::internal::SuiteApiResolver<XitTest>::GetSetUpCaseOrSuite(file, line),
            ::testing::internal::SuiteApiResolver<XitTest>::GetTearDownCaseOrSuite(file, line),
            // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
            new Factory(to_tag(suite_id, test_id, file_info), &inputs)
        );
    }

    template <typename T>
    void install(
        test::App& app,
        const std::string& suite_id,
        const std::string& test_id,
        const FileInfo& file_info
    )
    {
        install<T>(
            app,
            nil::xalt::tlist<typename T::base_t>(),
            suite_id,
            test_id,
            file_info //
        );
    }

    template <typename T>
    void install(
        headless::Inputs& inputs,
        const std::string& suite_id,
        const std::string& test_id,
        const FileInfo& file_info,
        const char* file,
        int line
    )
    {
        install<T>(
            inputs,
            nil::xalt::tlist<typename T::base_t>(),
            suite_id,
            test_id,
            file_info,
            file,
            line //
        );
    }
}
