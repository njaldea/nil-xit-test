#pragma once

#include "../Test.hpp"
#include "../headless/Inputs.hpp"
#include "../type.hpp"

#include <nil/xit/test/App.hpp>

#include <gtest/gtest.h>

#include <string>
#include <utility>

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
        using inputs_t = typename base_t::inputs_t;
        using outputs_t = typename base_t::outputs_t;

        const auto tag = to_tag(suite_id, test_id, dir);
        app.add_info(tag, {I.value...}, {O.value...});

        constexpr auto node //
            = [](const nil::gate::Core& core,
                 nil::gate::async_outputs<frame_t<O>...> asyncs,
                 bool enabled,
                 const frame_t<I>&... args)
        {
            if (enabled)
            {
                std::tuple<frame_t<O>...> result;
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
                {
                    auto batch = core.batch(asyncs);
                    [&]<std::size_t... i>(std::index_sequence<i...>)
                    {
                        (([&](){
                            auto* output = get<i>(batch);
                            output->set_value(std::move(std::get<i>(result)));
                        })(), ...);
                    }(std::make_index_sequence<sizeof...(O)>());
                }
                core.commit();
            }
        };
        app.add_node(
            tag,
            node,
            std::make_tuple(app.get_input<frame_t<I>>(frame_v<I>)...),
            std::make_tuple(app.get_output<frame_t<O>>(frame_v<O>)...)
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

                auto input_d = std::make_tuple(inputs->get<frame_t<I>>(frame_v<I>, tag)...);
                auto output_d = std::make_tuple(frame_t<O>()...);
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

    template <typename T>
    void install(
        test::App& app,
        const std::string& suite_id,
        const std::string& test_id,
        const std::string& dir
    )
    {
        install<T>(app, type<typename T::base_t>(), suite_id, test_id, dir);
    }

    template <typename T>
    void install(
        headless::Inputs& inputs,
        const std::string& suite_id,
        const std::string& test_id,
        const std::string& dir,
        const char* file,
        int line
    )
    {
        install<T>(inputs, type<typename T::base_t>(), suite_id, test_id, dir, file, line);
    }
}
