#pragma once

#include "../Test.hpp"
#include "../headless/CacheManager.hpp"

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

    template <typename T, typename I, typename O, typename E>
    void run_test(I& inputs, O& outputs, E& expects)
    {
        try
        {
            T p;
            p.setup();
            p.run(inputs, outputs, expects);
            p.teardown();
        }
        catch (const std::exception&)
        {
            // exception is thrown
        }
        catch (...)
        {
            // unknown exception is thrown
        }
    }

    template <typename T>
    void install(
        test::App& app,
        std::string_view suite_id,
        std::string_view test_id,
        const FileInfo& file_info
    )
    {
        using input_frames = typename T::input_frames;
        using output_frames = typename T::output_frames;
        using expect_frames = typename T::expect_frames;
        [&]<xalt::literal... I, xalt::literal... O, xalt::literal... E>(
            xalt::tlist<Inputs<I...>> /* inputs */,
            xalt::tlist<Outputs<O...>> /* outputs */,
            xalt::tlist<Expects<E...>> /* expects */
        )
        {
            using detail::Frame;

            const auto tag = app.add_info(
                to_tag(suite_id, test_id, file_info),
                {Frame<I>::marked_value...},
                {Frame<O>::marked_value...},
                {Frame<E>::marked_value...}
            );

            constexpr auto node
                = [](const Frame<I>::type&... input_args, const Frame<E>::type&... expect_args)
            {
                auto inputs = typename input_frames::type{{&input_args...}};
                auto result = std::make_tuple(typename Frame<O>::type()..., expect_args...);

                auto outputs = [&]<std::size_t... OI>(std::index_sequence<OI...>) //
                {                                                                 //
                    return typename output_frames::type{&get<OI>(result)...};
                }(std::make_index_sequence<sizeof...(O)>());

                auto expects = [&]<std::size_t... OE>(std::index_sequence<OE...>) //
                {                                                                 //
                    return typename expect_frames::type{&get<OE + sizeof...(O)>(result)...};
                }(std::make_index_sequence<sizeof...(E)>());

                run_test<T>(inputs, outputs, expects);
                return result;
            };

            app.add_node(
                tag,
                node,
                std::make_tuple(app.get_input<typename Frame<I>::type>(Frame<I>::value)...),
                std::make_tuple(app.get_output<typename Frame<O>::type>(Frame<O>::value)...),
                std::make_tuple(app.get_expect<typename Frame<E>::type>(Frame<E>::value)...)
            );
        }(xalt::tlist<typename T::input_frames>(),
          xalt::tlist<typename T::output_frames>(),
          xalt::tlist<typename T::expect_frames>());
    }

    template <typename T>
    void install(
        headless::CacheManager& cache_manager,
        std::string_view suite_id,
        std::string_view test_id,
        const FileInfo& file_info,
        const char* file,
        int line
    )
    {
        using input_frames = typename T::input_frames;
        using output_frames = typename T::output_frames;
        using expect_frames = typename T::expect_frames;
        [&]<xalt::literal... I,
            xalt::literal... O,
            xalt::literal... E>( //
            xalt::tlist<Inputs<I...>> /* inputs */,
            xalt::tlist<Outputs<O...>> /* outputs */,
            xalt::tlist<Expects<E...>> /* expects */
        )
        {
            class XitTest: public ::testing::Test
            {
            public:
                XitTest(std::string init_tag, headless::CacheManager* init_cache_manager)
                    : tag(std::move(init_tag))
                    , cache_manager(init_cache_manager)
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
                    using detail::Frame;
                    auto input_d = std::make_tuple(
                        cache_manager->get<typename Frame<I>::type>(Frame<I>::value, tag)...
                    );
                    auto expect_d = std::make_tuple(
                        cache_manager->get<typename Frame<E>::type>(Frame<E>::value, tag)...
                    );
                    auto output_d = std::make_tuple(typename Frame<O>::type()...);
                    auto inputs = std::apply(
                        [](const auto&... ii) { return typename input_frames::type{{&ii...}}; },
                        input_d
                    );
                    auto outputs = std::apply(
                        [](auto&... oo) { return typename output_frames::type{{&oo...}}; },
                        output_d
                    );
                    auto expects = std::apply(
                        [](auto&... ee) { return typename expect_frames::type{{&ee...}}; },
                        expect_d
                    );
                    run_test<T>(inputs, outputs, expects);
                }

                std::string tag;
                headless::CacheManager* cache_manager;
            };

            struct Factory: ::testing::internal::TestFactoryBase
            {
                Factory(std::string init_tag, headless::CacheManager* init_cache_manager)
                    : tag(std::move(init_tag))
                    , cache_manager(init_cache_manager)
                {
                }

                ::testing::Test* CreateTest() override
                {
                    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
                    return new XitTest(tag, cache_manager);
                }

                std::string tag;
                headless::CacheManager* cache_manager;
            };

            testing::internal::MakeAndRegisterTestInfo(
                std::string(suite_id),
                to_tag_suffix(test_id, file_info).c_str(),
                nullptr,
                nullptr,
                ::testing::internal::CodeLocation(file, line),
                ::testing::internal::GetTestTypeId(),
                ::testing::internal::SuiteApiResolver<XitTest>::GetSetUpCaseOrSuite(file, line),
                ::testing::internal::SuiteApiResolver<XitTest>::GetTearDownCaseOrSuite(file, line),
                // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
                new Factory(to_tag(suite_id, test_id, file_info), &cache_manager)
            );
        }(xalt::tlist<typename T::input_frames>(),
          xalt::tlist<typename T::output_frames>(),
          xalt::tlist<typename T::expect_frames>());
    }
}
