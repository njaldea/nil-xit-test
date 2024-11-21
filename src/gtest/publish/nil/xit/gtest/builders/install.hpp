#pragma once

#include "../Test.hpp"
#include "../headless/Inputs.hpp"
#include "../type.hpp"

#include <gtest/gtest.h>

#include <string>

namespace nil::xit::gtest::builders
{
    std::string to_tag(
        const std::string& suite_id,
        const std::string& test_id,
        const std::string& dir
    );

    template <typename P, StringLiteral... I, StringLiteral... O>
    void install(
        test::App& app,
        type<Test<Input<I...>, Output<O...>>> /* type */,
        const std::string& suite_id,
        const std::string& test_id,
        const std::string& dir
    )
    {
        using base_t = Test<Input<I...>, Output<O...>>;
        app.add_node(
            to_tag(suite_id, test_id, dir),
            [](const typename Frame<I>::type&... args) -> std::tuple<typename Frame<O>::type...>
            {
                using inputs_t = typename base_t::inputs_t;
                using outputs_t = typename base_t::outputs_t;
                std::tuple<typename Frame<O>::type...> result;
                try
                {
                    P p;
                    p.setup();
                    auto inputs = inputs_t{{&args...}};
                    auto outputs
                        = std::apply([](auto&... o) { return outputs_t{{&o...}}; }, result);
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
            },
            std::make_tuple(app.get_input<typename Frame<I>::type>(Frame<I>::value)...),
            std::make_tuple(app.get_output<typename Frame<O>::type>(Frame<O>::value)...)
        );
    }

    template <typename P, StringLiteral... I, StringLiteral... O>
    void install(
        headless::Inputs& inputs,
        type<Test<Input<I...>, Output<O...>>> /* type */,
        const std::string& suite_id,
        const std::string& test_id,
        const std::string& dir,
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
                auto input_data = std::make_tuple( //
                    inputs->get<typename Frame<I>::type>(Frame<I>::value, tag)...
                );
                auto output_data = std::make_tuple( //
                    typename Frame<O>::type()...
                );
                auto i = std::apply(
                    [](const auto&... ii) { return inputs_t{{&ii...}}; },
                    input_data //
                );
                auto o = std::apply(
                    [](auto&... oo) { return outputs_t{{&oo...}}; },
                    output_data //
                );
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
            (test_id + '[' + dir + ']').c_str(),
            nullptr,
            nullptr,
            ::testing::internal::CodeLocation(file, line),
            ::testing::internal::GetTestTypeId(),
            ::testing::internal::SuiteApiResolver<XitTest>::GetSetUpCaseOrSuite(file, line),
            ::testing::internal::SuiteApiResolver<XitTest>::GetTearDownCaseOrSuite(file, line),
            // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
            new Factory(to_tag(suite_id, test_id, dir), &inputs)
        );
    }
}
